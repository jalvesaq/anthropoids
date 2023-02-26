
# This is an example of how to create pgm and pbm files with R.  You can open
# pgm and pbm files with a text editor and read their manual pages to understand
# the formats.

# You can also use gimp to create pgm files (choose the option save as
# ascii). It will be necessary to manually delete the line starting with "#"
# inserted as a comment by gimp.

matrix2pbm <- function(m, file = "Rgrid.pbm") {
  if (max(m) > 1) stop("PBM files has only two colors.")
  x <- dim(m)[1]
  y <- dim(m)[2]
  sink(file)
  cat("P1\n", x, " ", y, "\n", sep = "")
  for (i in 1:x){
    for (j in 1:y){
      cat(m[i, j], " ", sep = "")
    }
    cat("\n", sep = "")
  }
  sink()
}

matrix2pgm <- function(m, file = "Rgrid.pgm") {
  if (max(m) == 1)
      stop("Use matrix2pbm() function to create files with only two colors.")
  x <- dim(m)[1]
  y <- dim(m)[2]
  sink(file)
  cat("P2\n", x, " ", y, "\n", max(m), "\n", sep = "")
  for (i in 1:x){
    for (j in 1:y){
      cat(m[i, j], " ", sep = "")
    }
    cat("\n", sep = "")
  }
  sink()
}


z <- vector(mode = "numeric", length = 10000)
z <- z + 1
g <- matrix(data = z, nrow = 100, ncol = 100)

for (i in 25:35){
  for (j in 1:100){
    g[i, j] <- 0
  }
}

for (i in 60:70){
  for (j in 1:100){
    g[i, j] <- 0
  }
}

for (j in 25:35){
  for (i in 1:100){
    g[i, j] <- 0
  }
}

for (j in 60:70){
  for (i in 1:100){
    g[i, j] <- 0
  }
}

matrix2pbm(g, file = "gridTemplate.pbm")
