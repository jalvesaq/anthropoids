
# Anthropoids

[Jakson Alves de Aquino](https://dadoseteorias.blogspot.com)  
 

## About

Anthropoids is an agent-based model of the evolution of cooperation among
virtual anthropoids. I presented an early version of it in my PhD thesis in
2008 (written in Objective C and using
[Swarm](http://www.swarm.org/wiki/Main_Page), the code is attached to this
[PDF](https://repositorio.ufmg.br/bitstream/1843/VCSA-7FFHRY/1/jakson_aquino_2008_tese_evolucao_da_cooperacao.pdf))
and the final version in an article published in 2011 (written in C++ and
using [Gtkmm](https://gtkmm.org/en/index.html), it can be found on [my
personal home page](http://dadoseteorias.blogspot.com/p/about-me.html)). The
original graphical interface no longer compiles on Ubuntu Linux, and I decided
to upgrade the Gtkmm code from version 2 to version 3. I replaced the GNU
build system with two simple Makefiles for Linux because the old `autogen.sh`
and `configure` scripts did not work anymore and I did not know how to fix
them. The R scripts required minor adjustments to work again, and I reindented
the whole C++ and R code. The model itself remains the same.

The model is overly complex. If someone wanted to continue to investigate the
evolution of cooperation using this model as a starting point, the next step
would be to simplify it rather than add new features.

I have not developed new agent-based models since 2008. However, exploring this
method of theoretical research again in the future is a possibility...

 

## Compile

Under Ubuntu and other Linux distributions derived from Debian, the following
command will install all necessary packages to compile and run anthropoids:

```
sudo apt install pkg-config make g++ libgtkmm-3.0-dev libgsl-dev zathura
```

R and the R packages `descr` and `igraph` have to be installed to see some
graphics during simulations.

After installing the dependencies, open a terminal emulator, go to
the `anthropoids` directory, and type:

```
make
sudo make install
```
 

## Run

With anthropoids already compiled and installed, type in the terminal:

```
anthropoids
```

While the program is running, you can use the mouse's left button to click on
agents and see what happens with them. The application will show a list of
agents present in the selected cell. If you click on one of the listed agents,
the program will draw lines linking the agent and all other agents that it
remembers that are not in the same cell. Blue lines represent positive
remembrances; red lines, negative remembrances; and, white lines, neutral. The
events happening with the selected agent will be printed in the terminal.

You can generate a PDF file with graphics by clicking on the
corresponding button (and waiting\...).

## Change parameters

If you do not change the parameters, each simulation will be a
repetition of the previous one. To get different "histories", it is necessary
to change the random seed passing the parameter `-s` to anthropoids
followed by a number. Example:

    ./anthropoids -s 237243

Using a different seed does not change the average initial values of the
variables: you have to edit the file `parameters`. This file is
created when you run anthropoids for the first time.

It is possible to avoid the creation of terrestrial herbaceous vegetation
and trees in some regions of the world by editing the picture
`gridTemplate.pbm`. No vegetation will be created under the
coordinates corresponding to the black color. The file
`gridTemplate.pbm` might be created by using R (as exemplified in the
script createGrid.R) or by using an image manipulation program, like The
Gimp.

 

## Collect data for statistical analysis

To collect data, edit the file `parameters` and change to an integer
number higher than 0 the value of LogInterval. During the simulation,
the files `completeRun.csv` and `deadOnes.csv` will be continuously
fed with data. These files have, respectively, general data and detailed
information about the agents who died in the last cycle. There are other
two files. One of them is `entirePop.csv`, which has almost the
same variables as `deadOnes.csv`, but with data from agents that were
alive when the file was created. The other is `agRemembrances.csv`,
which has the all remembrances of all agents that were alive when the
file was created, excluding the remembrances that live agents keep of
already dead ones. These two files are generated at the end of the
simulation, but they can also be created at any time by sending the
signal USR2 to anthropoids. You can send this signal with the following
command:

    killall -s USR2 anthropoids

Without parameters, the command `killall` kills the program by sending
the SIGTERM signal. The option `-s USR2` means that the signal to be
sent must be `USR2` (Second User Defined Signal).

You can run anthropoids without graphics. To run anthropoids in batch mode,
you have to add option `-b`, as below:

    ./anthropoids -b -s <number>

In this case, the simulation duration will be defined by the value of
`ExperimentDuration` which might be defined in the file
`parameters`. If you do not want to wait for `ExperimentDuration`,
you can terminate the simulation at any moment with the command:

    killall -s USR1 anthropoids
