#include "lyra_sendreceive.hpp"

#include "../core/mesh.hpp"
#include "../core/point.hpp"
#include "../core/tools.hpp"

void
LyraSend (Mesh *mesh, std::vector<real_t> &values)
{
    ul_t totalsize = mesh->GetTotalNumberOfSenderPoints ();

    if (values.size () != totalsize)
    {
        ERROR << "not good parameters in LyraSend... total size = " << totalsize << " values size = " << values.size ()
              << ENDLINE;
        return;
    }

    if (totalsize == 0)
        return;

    real_t *cursor = &values [0];
    for (ul_t idProc = 0; idProc < USIGNED (ProcMe->nproc); ++idProc)
    {
        if (idProc == USIGNED (ProcMe->rank))
            continue;

        ul_t size = mesh->GetNumberOfSenderPointsOnProc (idProc);

        if (size == 0x0)
            continue;

        MPI_Send (cursor, static_cast<int> (size), MPI_REAL_T, static_cast<int> (idProc), ProcMe->rank, MPI_COMM_WORLD);

        //        STATUS << ProcMe->rank << " send to " << idProc << " : " << size << " values" << ENDLINE;

        cursor += size;
    }

    return;
}

void
LyraRecv (Mesh *mesh, std::vector<real_t> &values)
{
    if (mesh->GetTotalNumberOfReceiverPoints () == 0)
        return;

    values.resize (mesh->GetTotalNumberOfReceiverPoints ());

    real_t *cursor = &values [0];
    for (ul_t idProc = 0; idProc < USIGNED (ProcMe->nproc); ++idProc)
    {
        if (idProc == USIGNED (ProcMe->rank))
            continue;

        ul_t size = mesh->GetNumberOfReceiverPointsOnProc (idProc);

        if (size == 0x0)
            continue;
        //        STATUS << ProcMe->rank << " receive from " << idProc << " : " << size << " values" << ENDLINE;

        //        MPI_Status status;
        MPI_Recv (cursor, static_cast<int> (size), MPI_REAL_T, static_cast<int> (idProc), static_cast<int> (idProc),
                  MPI_COMM_WORLD, static_cast<MPI_Status *> (nullptr));

        //        INFOS << "Proc " << ProcMe->rank << " receive from " << idProc << " " << ENDLINE;

        cursor += size;
    }

    return;
}
#include <fstream>

error_t
LyraCheckCommunications (Mesh *mesh)
{
    LYRA_BARRIER;
    if (LYRA_ASK)
        BEGIN << "Check communications" << ENDLINE;
    LYRA_BARRIER;

    bool good = true;
    for (int idProc = 0; idProc < ProcMe->nproc; ++idProc)
    {
        if (idProc != ProcMe->rank)
        {
            ul_t meToReceive = mesh->GetReceiverPointsOnProc (USIGNED (idProc))->size ();
            ul_t meToSend    = mesh->GetSenderPointsOnProc (USIGNED (idProc))->size ();

            ul_t targetToSend = 0;

            MPI_Send (&meToSend, 1, MPI_UNSIGNED_LONG, idProc, 0, MPI_COMM_WORLD);
            MPI_Recv (&targetToSend, 1, MPI_UNSIGNED_LONG, idProc, 0, MPI_COMM_WORLD, nullptr);

            if (targetToSend != meToReceive)
            {
                ERROR << "The proc " << REVERSE << ProcMe->rank << COLOR_RED << " wait to receive " << meToReceive
                      << " and target " << REVERSE << idProc << COLOR_RED << " sends " << targetToSend << "..."
                      << ENDLINE;

                good = false;
            }
        }
    }

    if (!good)
    {
        ERROR << "a difference has been detected on proc " << ProcMe->rank << ENDLINE;

        return EXIT_FAILURE;
    }
    INFOS << "correct number of senders/receivers on proc " << ProcMe->rank << ENDLINE;

    LYRA_BARRIER;

    for (int idProc = 0; idProc < ProcMe->nproc; ++idProc)
    {
        if (idProc != ProcMe->rank)
        {
            ul_t meToReceive = mesh->GetReceiverPointsOnProc (USIGNED (idProc))->size ();
            ul_t meToSend    = mesh->GetSenderPointsOnProc (USIGNED (idProc))->size ();

            std::vector<ul_t> meToSendIdx    = {};
            std::vector<ul_t> meToReceiveIdx = {};

            meToSendIdx.reserve (meToSend);
            meToReceiveIdx.reserve (meToReceive);

            for (Point *p : *mesh->GetSenderPointsOnProc (USIGNED (idProc)))
                meToSendIdx.push_back (p->globalId);

            for (Point *p : *mesh->GetReceiverPointsOnProc (USIGNED (idProc)))
                meToReceiveIdx.push_back (p->globalId);

            std::vector<ul_t> targetToSendIdx (meToReceive, 0x0);

            MPI_Send (&meToSendIdx [0], static_cast<int> (meToSend), MPI_UNSIGNED_LONG, idProc, 0, MPI_COMM_WORLD);

            MPI_Recv (&targetToSendIdx [0], static_cast<int> (meToReceive), MPI_UNSIGNED_LONG, idProc, 0,
                      MPI_COMM_WORLD, nullptr);

            std::vector<std::pair<ul_t, ul_t>> diff = {};
            for (ul_t idvec = 0; idvec < meToReceive; ++idvec)
                if (meToReceiveIdx [idvec] != targetToSendIdx [idvec])
                    diff.push_back ({meToReceiveIdx [idvec], targetToSendIdx [idvec]});

            for (std::pair<ul_t, ul_t> pdiff : diff)
            {
                ERROR << "[" << ProcMe->rank << "] need " << pdiff.first << " & receive [" << idProc << "] "
                      << pdiff.second << ENDLINE;

                good = false;
            }
        }
    }

    if (!good)
    {
        ERROR << "dissimilar target has been detected on proc " << ProcMe->rank << ENDLINE;

        return EXIT_FAILURE;
    }
    INFOS << "correct targets on proc " << ProcMe->rank << ENDLINE;

    LYRA_BARRIER;

    if (LYRA_ASK)
        ENDFUN;

    return EXIT_SUCCESS;
}
