
c1 <- 8 # constant
c2 <- 3 # constant

calcP <- function(n, e) {
    p <- 1 - 2 ^ ((-1.0) * n * c1 / e)
    p <- p - (p * (1 / (c2 * n)))
    p
}

calcR <- function(n, e) {
    r <- calcP(n, e) * e / n
    r
}


n <- c(1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 6, 6, 6, 8, 8, 8, 12, 12, 12)
e <- rep(c(4, 30, 60),  7)

p <- vector(length = 21, mode = "numeric")

for (i in 1:21) p[i] <- calcP(n[i], e[i])

r <- e * p / n

b <- as.data.frame(cbind(n, e, p, r))

n <- 1:20
e <- 1:20
e <- e * 3
p <- outer(n, e, calcP)
r <- outer(n, e, calcR)
pcol <- matrix(nrow = 19, ncol = 19)
rcol <- matrix(nrow = 19, ncol = 19)
r.max <- max(r)

for (i in 1:19) {
    for (j in 1:19) {
        pcol[i, j] <- rgb((1 - p[i, j]),  1,  p[i,  j])
        rcol[i, j] <- rgb((1 - (r[i, j] / r.max)),  1,  (r[i,  j] / r.max))
    }
}

par(mai = c(0, 0.0, 0, 0.0))
png(file = "expectedMeat.png", width = 620, height = 480, pointsize = 14)
persp(n, e, r, theta = -30, expand = 0.5, col = pcol, ticktype = "detailed",
      xlab = "number of hunters", ylab = "energy of prey",
      zlab = "expected value")
dev.off()

quit(save = "no")
