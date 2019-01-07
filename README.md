# EM-seq

EM-seq is a program implementing an expectation (E-M) framework to perform a probabilistic, unsupervised, classification of a set of DNA sequences into  a user defined number of classes. The classes are modeled using mononucleotide base probability matrices (also sometimes called position weight matrices (PWM) in the litterature). These matrices are a summary of the over-represented sub-sequences in each class and, de facto, represent sequence motifs. The classification is iteratively optimized following a two step procedure : i) during the expectation step (E-step), the sequences are probabilistically assigned (soft clustering) to all the classes given the current class models which in turn allows to ii) update the class matrices given the sequence class assignments during the maximization step (M-step). The process is iterated until reaching convergence (the sequence class assignment is stable from one iteration to the next) or the maximum number of iterations.

During the classification part, the class motifs atr searched at each possible offset along the input DNA sequences. That is, if the DNA sequences have a length L and the class motifs have a length L'&lt; L, each motif is searched at L-L'+1 positions along each sequence. Additionally, the motifs search can be extanded to both strands.


## Seeding strategies

This program only performs a classification optimization. As a matter of fact, the classification needs to be initialized (or seeded). So far, two seeding strategies are available.
First, the procedure can be initialised at random using "--seeding random". In this case, all the sequences are initially assigned to classes randomly. Probabilities are randomly initialised using a beta distribution. The initial motifs are then computed from the initial sequence assignments. For the sake of reproducibility, a seed initializing the random number generator can be specified using 
the "--seed" option.
Second, it is possible to provide one or more files containing motifs which should be used as starting motifs. The motif format should be a simple text file with 4 rows corresponding to A,C,G and T and the columns should be separated by any number of blank (non-eol) characters. In this case, the file can be given using "--seeding &lt;file1&gt;,&lt;file2&gt;,". The list is expected to contain a final coma, even if there is only one file.


## Prerequisites

In order to compile and run EM-seq, you need to have Scons and three C++ libraries installed on your machine :

	1) Scons v2.3.0 or higher to compile all the program listed above(https://scons.org/pages/download.html) 
	2) SFML v2.4.2 or higher (see https://www.sfml-dev.org/download/sfml/2.4.2/)
	3) boost v1.4.1 or higher (https://www.boost.org/)
	4) UnitTest++ v2.0 or higher (see https://github.com/unittest-cpp)


The Scons configuration files SConstruct and SConscript are configured such that they will look for :
		
	1) boost static libaries in /usr/local/lib/boost and boost header files in /usr/local/include/boost
	2) UnitTest++ static libaries in /usr/local/lib/UnitTest++ and UnitTest++ header files in /usr/local/include/UnitTest++

so it is highly recommanded to install these libraries here. Another solution is to modify the SConscript file (located in src/)
to adapt the library paths (modify the lib_unittest_path, lib_boost_path and lib_sfml_path variable values).

Be aware that each library may have its own dependencies (this is especially true for the SFML library, see 
https://www.sfml-dev.org/tutorials/2.5/compile-with-cmake.php)


## Compiling

Once every library has been installed, simply cd into the EM_sequence directory, at the level of the SConstruct file and execute :

```
scons
```

If the compilation worked, you should see a bin/ directory containing the following executables :
	1) em_seq the classification program
	2) unittests a set of test cases

If the compilation worked properly, you should be able to run some tests by runnning :

```
bin/unittests
```
and by displaying the program help by running : 

```
bin/em_seq --help
```

## Input format

The input data are provided using the "--data" option. Two file formats are accepted, for conveniance :

	1) fasta files
	2) character matrix files

Fasta files are expected to be regular fasta files. The headers should be present but are ignored during the classification, only the sequences are considered. The character matrix file is a file containing one sequence per row, with each character being separated from its neighbours by any number of blank (non-eol) characters. In both cases, all the sequences should have the same length and the alphabet is strictly restricted to A, C, G and T.

Here is an example of a proper input in fasta format :

```
>sequence1
GACTAATACGCTTGTGCTCCGCCGGTTACC
>sequence2
GGTGATATTGAAGCGGCATCGTTAGACCAC
>sequence3
GTTACGGTGCACTAGCCTTAGATCGTCAAA
>sequence4
TGTCTATGGAGGTTTGGAGGATCTTGATGG
```
and the corresponding data in character matrix format :

```
G A C T A A T A C G C T T G T G C T C C G C C G G T T A C C
G G T G A T A T T G A A G C G G C A T C G T T A G A C C A C
G T T A C G G T G C A C T A G C C T T A G A T C G T C A A A
T G T C T A T G G A G G T T T G G A G G A T C T T G A T G G
```


## Output format

Once a dataset has been classified, the program reports the class motif and the posterior class probabilities.

### Class motifs
Each class motif is written in a text file (the directory is specified as a program option) as a Lx4 matrix where the rows represent the motif positions and the columns A,C,G and T respectively. The files are named "&lt;prefix&gt;\_motif\_&lt;class\_id&gt;.mat" where &lt;class\_id&gt; correspond to the class number and &lt;prefix&gt; to the value passed to the "--write" option.

### Probabilities
As this program performs a soft clustering, several set of posterior probabilities are returned as matrices in text files.

#### Posterior probabilies
The posterior probabilities are returned in a text file named "&lt;prefix&gt;\_postprob.mat" where &lt;prefix&gt; corresponds to the value passed to the "--write" option. The posterior probabilities represent the probabilities of each sub-sequence of length L', in each sequence of length L, to belong to each possible class (in each orientation if the search was extended to both strands). That takes a 3D matrix to represent these probabilities and a 4D matrix if the search was extended to both strands. In all cases, the matrix dimension are N x K x O x F where N is the number of sequences, K the number of classes, O=L-L'+1 the number of offsets and F=2 if both strands were searched or F=1 otherwise. The matrix is printed using R format for 4D arrays. The background class, if there is one, is always the last class. Finally, note that all the probability values sum up to 1.

#### Class probabilities
The class probability, at each offset, are returned in a file named "&lt;prefix&gt;\_classprob.mat" where &lt;prefix&gt; corresponds to the value passed to the "--write" option. The matrix dimensions are K x O x F (see "Posterior Probabilities" paragraph for the variable descriptions). The background class, if there is one, is always the last class. Finally, note that all the probability values sum up to 1.

#### Overall class probabilities
The overall class probabilities, that is the class weights, are returned and written in a file named "&lt;prefix&gt;\_classproboverall.mat". The matrix dimensions are 1 x K (see "Posterior Probabilities" paragraph for the variable descriptions). The background class, if there is one, is always the last class. Finally, note that all the probability values sum up to 1.

### Graphical interface
For convenience the different class motifs can be displayed as logos through a graphical interface (which is enabled by default). This functionality, which can be turned off, is primariliy designed to be used when performing data exploration and quick parameters fine tunning.


## Running an example

A toy example is provided in the example/ folder. It contains simulated sequences. The dataset, stored in "example/data_2motifs.mat", contains two classes. Each class is characterized by a given motif (which are stored in "example/motif1.mat" and "example/motif2.mat") which occure only once, at any offset, on both strands, in the sequences. Each sequence belongs to one or the other class and never to both at the same time. 70% of the sequences contains motif 1 and 30% motif 2.

A classification of the sequences can be run using :
```
bin/em_seq --data example/data_2motifs.mat --classes 2 --iter 300 --length 8 --flip --bgclass --nogui --write example/results --seeding random --seed 2018may28
```
Note that i) we are looking for motifs of length 8, which is exactly the length of the motifs (see the class motifs) and that ii) we are enabling a background class even though we know that all the sequences bear a motif (and belong to a class).

To plot the results and compare them with the true motifs, you can use the R script "example/plot.R". To run this R script, you will need to have motifStack and MotifDb libraries, from BioConductor, installed. Normally you should see a tree with 4 logos. You should see that each of the found motifs cluster together with one of the true motifs, indicating the match between them. The background class motif logo should be empty (indicating a uniform probability distribution) and should be far from the two other groups.


## Options
  | short | long&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | description |
  |:------|:--------------------|:---------------------------|
  | \-h   | \-\-help    |  Displays the help. |
  | \-v   | \-\-version |   Displays the version number. |
  | \-d   | \-\-data    | Specifies the file containing the DNA sequences to classify. For the file format supported, please read to section 2). |
  |       | \-\-fasta   | Specifies that the file given using \-\-data is in fasta format. |
  |       | \-\-from    | Allows to restrict the data used to a subset of the entire dataset. This option specifies the position (1\-based) of the first character of each sequence which should be used(including this one). By default, the 1st character used is the 1st one of each sequence (equivalent to use \-\-from \-1). |
  |       | \-\-to      | Allows to restrict the data used to a subset of the entire dataset. This option specifies the position (1\-based) of the last character of each sequence which should be used (including this one). By default, the last character used is the last one of each sequence (equivalent to use \-\-from \-1). |
  | \-i   | \-\-iter    | Specifies the maximum number of optimizing iterations. |
  | \-l   | \-\-length  | Specifies the length of the motif to train, in number of bases. |
  | \-c   | \-\-class   | Specifies the number of classes to use to classify the sequences. By default 1. |
  |       | \-\-bgclass | Allows to include an extra class (additionally to the ones defined using \-\-class). This class serves to model the background and has a motif having values equal to the background probability of each base. The background class motif has a length equal to the other classes and is not subjected to optimization (it remains the same during the whole process). The background class is always the last one in the results. |
  |       | \-\-write   | Instructs the program to write the results in files named "&lt;arg&gt;\_motif\_&lt;class\_id&gt;.mat" for the motifs, "&lt;arg&gt;\_postprob.mat" for the posterior probabilities, "&lt;arg&gt;\_classprob.mat for the class probabilities and &lt;arg&gt;\_classproboverall.mat for the overall class probabilies. |
  |       | \-\-nogui   | Disable the motif displays at the end. |
  |       | \-\-seeding | Specifies the seeding strategy. For more informations, please read section 3). |
  |       | \-\-seed    | Specifies a seed to initialize the random number generator, usefull when using a random seeding strategy. |



## Authors

* **Romain Groux**


## License

This project is licensed under the GNU General Public License v3 - see the [LICENSE.md](LICENSE.md) file for details


## Acknowledgments

* René Dreos
* Philipp Bucher
* Giovanna Ambosini


