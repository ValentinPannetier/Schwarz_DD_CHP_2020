
#  The Lyra Project ~ Schwarz_DD_CHP_2020 
Under the supervision of  [Mrs Beaugendre](https://www.math.u-bordeaux.fr/~hbeaugen/), 2020, Université de Bordeaux & Matmeca Bordeaux.
  ***Subject :** HPC project, Schwarz decomposition*.
    
##  ⚒ To compile
Please create a **build** directory at the root of the project.  Then call cmake inside by  `cmake ..` (some options must be activated in order to be able to use [Metis](http://glaros.dtc.umn.edu/gkhome/metis/metis/overview) or [Scotch](https://www.labri.fr/perso/pelegrin/scotch/) for example, look on the side of ` cmake-gui ..`)

To compile `make` (if Metis and / or Scotch are enabled, cmake will download the last version and install them in the **build** folder)

The standard order of use is

 1. generator-lyra.
 2. partition-lyra.
 3. solver-lyra.

## 🖥 generator-lyra
The Lyra mesh generator supports rectangular or square domains.  You must specify the dimensions in the x dimension and the y dimension (respectively **Lx** and **Ly**) and the number of points in these two directions (**Nx** and **Ny**). You must also give a **filename** / path to the output file (with a *lyra* extension).
For example,	`./generator-lyra 1 1 100 100  ../data/test.lyra`	will create a mesh written in `../data/test.lyra` of the square *[0, 1]x[0, 1]* with *100* points in each directions.

The boundary condition by default is a **Dirichlet condition**.

## 🖥 partition-lyra

The Lyra partitioner requires an input mesh in Lyra format (see the section on Lyra mesh format).
`./partition-lyra $filename.lyra $nparts $nrecover $ptag $partitionner`

 - `$filename.lyra` the input mesh (like `../data/test.lyra`)
 - `$nparts` the number of partitions needed
 - `$nrecover` the size of the interior covering for **one partition** (you will see it multiplied at least by two on the final mesh)

**The rule of the youngest** is used to deduce the single partition which must send its value in the case of a point covered by more than two partitions.

### Options `$ptag` (choose the condition between partitions)
 - `-vd` or `--dirichlet` Dirichlet boundary
 - `-vr` or `--robin` Robin (alpha = beta = 1)
 
**Be careful**, when you choose a Robin type condition, the corners on the partitions are treated as a Dirichlet condition.

### Options `$partitionner` (name of partitionner)
 - `-pm` or `--metis` use Metis, so Metis need to be download and build (see *To compile* part) 
 - `-ps` or `--scotch` use Scotch, so Scotch need to be download and build (see *To compile* part)
 - `-pv` or `--voronoi` equilibrate diagram of Voronoi. 
 - `-pc` or `--column` regular columns
 - `-pr` or `--row` regular rows
 - `-pb` or `--board` regular checkerboards

## 🖥 solver-lyra
`mpirun -np $n  ./solver-lyra ../data/test.$n` where `$n` is the number of processes on which to solve our problem (so this must also be the number of partitions created using `partition-lyra`).

A lot of features can be disable directly (no options here) as `LyraCheckCommunications` or `WriteVTKFile` in `IterateSchwarz`. You can also choose many criteria as `numSchwarz` and `epsilon` in `main.cpp`. You can also change analytical solution in the same file.

## 🗊 The Lyra mesh format
No comments are possible if this type of file everything is read !

    number-of-points
    coor-x coor-y coor-z global-num tag tag-infos \\ for each points
    ...
    
    number-of-cells
    vertex-1 vertex-2 vertex-3 vertex-4 \\ for each cell
    ...
    

 - The global num is used to send and receive values, so take care of it.
 - The tag is a binary number which represents all information tag on the points 
 
| Tag               | Value         | Digit | 
| ----------------- |:-------------:| -----:|
| `PT_NONE`         | 0x0           | 1     | 
| `PT_SHARED`       | 0x1           | 1     |  
| `PT_PHYSICAL`     | 0x2           | 2     |
| `PT_VIRTUAL`      | 0x4           | 3     |
| `PT_SEND`         | 0x8           | 4     |
| `PT_RECEIVE`      | 0x10          | 5     |
| `PT_DIRICHLET`    | 0x20          | 6     |
| `PT_NEUMANN`      | 0x40          | 7     |

In addtion, we have a special value for `PT_ROBIN` which is the bitwise **OR** of `PT_DIRICHLET` and `PT_NEUMANN`.  
 > For example *1101101* is a `PT_SHARED` and `PT_VIRTUAL` and  `PT_SEND` and `PT_ROBIN`
 
 - tag-infos is only determined by `PT_SEND` or `PT_RECEIVE`
-- if `PT_SEND`  it's `number-of-receveirs [id-proc dir-normal-outward] ...`,
-- if `PT_RECEIVE` it's only `id-proc-sender`.
