
# import libraries (these belongs to BioConductor)
library(motifStack)
library(MotifDb)

# true motif hidden in the data
true1 = as.matrix(read.table("example/motif_class1.mat"))
true2 = as.matrix(read.table("example/motif_class2.mat"))

rownames(true1) = c('A', 'C', 'G', 'T')
colnames(true1) = 1:ncol(true1)

rownames(true2) = c('A', 'C', 'G', 'T')
colnames(true2) = 1:ncol(true2)


# discovered motifs
motif1 = t(as.matrix(read.table("example/results_motif_class1.mat"))) # class 1
motif2 = t(as.matrix(read.table("example/results_motif_class2.mat"))) # class 2
motif3 = t(as.matrix(read.table("example/results_motif_class3.mat"))) # background class

rownames(motif1) = c('A', 'C', 'G', 'T')
colnames(motif1) = 1:ncol(motif1)

rownames(motif2) = c('A', 'C', 'G', 'T')
colnames(motif2) = 1:ncol(motif2)

rownames(motif3) = c('A', 'C', 'G', 'T')
colnames(motif3) = 1:ncol(motif3)


motifs = list()
motifs[[1]] = new("pcm", mat=motif1, name="true 1")
motifs[[2]] = new("pcm", mat=motif2, name="true 2")
motifs[[3]] = new("pcm", mat=motif1, name="discovered 1")
motifs[[4]] = new("pcm", mat=motif2, name="discovered 2")
motifs[[5]] = new("pcm", mat=motif3, name="discovered bckg")

motifStack(motifs, layout="tree")
