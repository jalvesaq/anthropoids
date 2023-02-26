file.rename("agRemembrances.csv", "agRemembrances.csv.orig")
system("head -n 2 agRemembrances.csv.orig > agRemembrances.csv")
system("awk '{ if ($9 == 1 && $20 > 0) print $0 }' agRemembrances.csv.orig >> agRemembrances.csv")
# Problem: remembered and remembering agents will not be exactly the same.
system("awk '{if ( $9 == 1 ) print $1 }' agRemembrances.csv.orig > agRemembNames")
system("sort -u agRemembNames > agRemembNames.csv")

library("igraph")

agNames <- readLines("agRemembNames.csv")
agNames <- unique(agNames)
nAgents <- length(agNames)
rm(agNames)

b1 <- read.table("agRemembrances.csv", header = TRUE)

arrowTail <- b1$myID - 1
arrowHead <- b1$otherID - 1
el <- cbind(arrowTail, arrowHead)
g <- graph.edgelist(el)
g.layout <- layout_with_fr(g)

plot(g, vertex.size = 1, layout = g.layout, edge.color = "darkgray",
     edge.arrow.size = 0.1, vertex.label = NA)


detach("package:igraph")


# The code below uses the package statnet which can no longer be installed from CRAN
quit(save = "no")

library("statnet")
agNames <- read.table("agRemembNames.csv")
agNames <- levels(agNames$V1)
nAgents <- length(agNames)
rm(agNames)

b1 <- read.table("agRemembrances.csv", header = TRUE)

len <- length(x$myID)
if (len > 0) {
    el <- matrix(ncol = 2, nrow = len)
    for (i in 1:len) {
        el[i, 1] <- x$myID[i]
        el[i, 2] <- x$otherID[i]
        ek[i] <- x$kinship
    }
    el
}


nt <- network.initialize(nAgents)
add.edges(nt, b1$myID, b1$otherID)


agSex <- vector(length =  nAgents, mode = "numeric")
b2 <- subset(b1, n == 1, select = c("agent", "myID", "sex1", "x1", "y1"))
tmpNames <- b2$agent
nni <- length(tmpNames)
for (i in 1:nni) {
    idx <- grep(tmpNames[i], b2$agent)
    id <- b2$myID[idx]
    agSex[id] <- as.numeric(b2$sex1[idx])
    if (agSex[id] == 1) agSex[id] <- 6  else agSex[id] <- 7 # blue or red
}
rm(b2, nni)
