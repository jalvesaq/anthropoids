
library(descr)

options(OutDec = ".")

# Colors: Stranger, Mother, Sibling,  Child,  Cyan,     light Red, light Blue,       8         9        10         11
#            1        2        3        4         5         6          7          Green,    Yellow,    Rose,     Magenta
palette(c("black",  "red",  "green3", "blue", "#BBFFFF", "#FFBBBB", "#AABBFF", "#BBFFBB", "#FFFFBB", "#FFDDDA", "#DDBBDD"))


# Are the datasets too big (yes = 1,  no = 0)?
TOOBIG <- 0

# Some of the simulation algorithms might not have been activated
FOODSHARE <- 0
HUNT <- 0
TERRITORY <- 0

pdf(file = "graphics.pdf", paper = "special", family = "Times", width = 5.8, height = 4)
par(ps = 10, pty = "m")

# For small runs, it might be useful use a valor lower than 100
TMPLENGTH <- 100
tmp <- vector(mode = "numeric", length = TMPLENGTH)

if (TOOBIG) {
    system("mv completeRun.csv completeRun.csv.original")
    system("statist --xsample 10 completeRun.csv.original > completeRun.csv")
}

c <- read.table("completeRun.csv", header = TRUE, as.is = TRUE)

if (TOOBIG) {
    system("mv completeRun.csv.original completeRun.csv")
}

cent <- round(TMPLENGTH * (c$time - min(c$time)) / (max(c$time) - min(c$time)))
sink("statistics")

cat("Complete Run\n\n")
summary(c)

# Number of agentes
for (i in 1:TMPLENGTH) tmp[i] <- mean(c$nF[cent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(c$nM[cent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("red", "blue"), xlab = "Time (% of total)", ylab = "Average number of agents")
rm(ts1, ts2)

# Hunt
if (max(c$nHunts) > 0) {
    HUNT <- 1
    for (i in 1:TMPLENGTH) tmp[i] <- mean(c$nHunts[cent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(c$nHunters[cent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(c$nFoundPreys[cent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(c$nSuccHunts[cent == i])
    ts4 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, ts4, col = c("blue", "green", "brown", "red"), xlab = "Time (% of total)", ylab = "Hunts")
    legend("topleft", legend = c("hunts", "hunters", "found preys", "succs. h."),
           lty = 1, col = c("blue", "green", "brown", "red"), bg = "white")
    rm(ts1, ts2, ts3, ts4)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(c$nPr[cent == i])
    ts1 <- ts(tmp)
    ts.plot(ts1, xlab = "Time (% of total)", ylab = "N. of Preys")
    rm(ts1)
}


# Food share
if (max(c$nSharedMM) > 0 && max(c$nSharedFF) > 0 && max(c$nSharedFM) > 0 && max(c$nSharedMF) > 0) {
    FOODSHARE <- 1
    for (i in 1:TMPLENGTH) tmp[i] <- mean(c$nBegsMM[cent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(c$nBegsFF[cent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(c$nBegsFM[cent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(c$nBegsMF[cent == i])
    ts4 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, ts4, col = c("black", "red", "green", "blue"), xlab = "Time (% of total)",
            ylab = "Average number of food requests")
    legend("topleft", legend = c("M -> M", "F -> F", "F -> M", "M -> F"), lty = 1,
           col = c("black", "red", "green", "blue"), bg = "white")
    rm(ts1, ts2, ts3, ts4)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(c$nSharedMM[cent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(c$nSharedFF[cent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(c$nSharedFM[cent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(c$nSharedMF[cent == i])
    ts4 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, ts4, col = c("black", "red", "green", "blue"), xlab = "Time (% of total)",
            ylab = "Average number of accepted food requests")
    legend("topleft", legend = c("M -> M", "F -> F", "F -> M", "M -> F"), lty = 1,
           col = c("black", "red", "green", "blue"), bg = "white")
    rm(ts1, ts2, ts3, ts4)


    t <- subset(c, (nBegsMM > 0), select = c("time", "nBegsMM", "nSharedMM"))
    scent <- round(TMPLENGTH * (t$time - min(t$time)) / (max(t$time) - min(t$time)))
    r <- t$nSharedMM / t$nBegsMM
    for (i in 1:TMPLENGTH) tmp[i] <- mean(r[scent == i])
    ts1 <- ts(tmp)
    rm(t, scent, r)

    t <- subset(c, (nBegsFF > 0), select = c("time", "nBegsFF", "nSharedFF"))
    scent <- round(TMPLENGTH * (t$time - min(t$time)) / (max(t$time) - min(t$time)))
    r <- t$nSharedFF / t$nBegsFF
    for (i in 1:TMPLENGTH) tmp[i] <- mean(r[scent == i])
    ts2 <- ts(tmp)
    rm(t, scent, r)

    t <- subset(c, (nBegsFM > 0), select = c("time", "nBegsFM", "nSharedFM"))
    scent <- round(TMPLENGTH * (t$time - min(t$time)) / (max(t$time) - min(t$time)))
    r <- t$nSharedFM / t$nBegsFM
    for (i in 1:TMPLENGTH) tmp[i] <- mean(r[scent == i])
    ts3 <- ts(tmp)
    rm(t, scent, r)

    t <- subset(c, (nBegsMF > 0), select = c("time", "nBegsMF", "nSharedMF"))
    scent <- round(TMPLENGTH * (t$time - min(t$time)) / (max(t$time) - min(t$time)))
    r <- t$nSharedMF / t$nBegsMF
    for (i in 1:TMPLENGTH) tmp[i] <- mean(r[scent == i])
    ts4 <- ts(tmp)
    rm(t, scent, r)

    ts.plot(ts1, ts2, ts3, ts4, lty = 1, col = c("black", "red", "green", "blue"),
            main = "Proportion of food shared to food asked", ylab = "Average value for each 1% of time",
            xlab = "Time (% of total)")
    legend("topleft", legend = c("M -> M", "F -> F", "F -> M", "M -> F"),
           col = c("black", "red", "green", "blue"), lty = 1, bg = "white")
    rm(ts1, ts2, ts3, ts4)
}

# Territory conflicts
if (max(c$NTConflicts) > 0) {
    TERRITORY <- 1

    for (i in 1:TMPLENGTH) tmp[i] <- mean(c$NTConflicts[cent == i])
    ts1 <- ts(tmp)
    ts.plot(ts1, xlab = "Time (% of total)", ylab = "Average value for each % of time",
            main = "Number of conflicts for territory")
    rm(ts1)

    t <- subset(c, (NTConflicts > 0), select = c("time", "NTConflicts", "TN1", "TN2", "TEnd1",
                                                 "TEnd2", "TEnd3", "TEnd4", "TEnd5", "TEnd6"))
    tcent <- round(TMPLENGTH * (t$time - min(t$time)) / (max(t$time) - min(t$time)))

    for (i in 1:TMPLENGTH) tmp[i] <- sum(t$TN1[tcent == i] / sum(t$NTConflicts[tcent == i]))
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- sum(t$TN2[tcent == i] / sum(t$NTConflicts[tcent == i]))
    ts2 <- ts(tmp)
    ts.plot(ts1, ts2, col = c("green", "magenta"), xlab = "Time (% of total)",
            ylab = "Average value for each % of time", main = "Number of conflicts for territory")
    legend("topleft", legend = c("NAllies1", "NAllies2"), lty = 1, col = c("green", "magenta"), bg = "white")
    rm(ts1, ts2)

    TEnd0 <- t$NTConflicts - t$TEnd1 - t$TEnd2 - t$TEnd3 - t$TEnd4 - t$TEnd5 - t$TEnd6
    for (i in 1:TMPLENGTH) tmp[i] <- mean(TEnd0[tcent == i]/t$NTConflicts[tcent == i])
    ts0 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(t$TEnd1[tcent == i]/t$NTConflicts[tcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(t$TEnd2[tcent == i]/t$NTConflicts[tcent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(t$TEnd3[tcent == i]/t$NTConflicts[tcent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(t$TEnd4[tcent == i]/t$NTConflicts[tcent == i])
    ts4 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(t$TEnd5[tcent == i]/t$NTConflicts[tcent == i])
    ts5 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(t$TEnd6[tcent == i]/t$NTConflicts[tcent == i])
    ts6 <- ts(tmp)
    ts.plot(ts0, ts1, ts2, ts3, ts4, ts5, ts6, col = c("black", "blue", "brown", "red", "yellow",
                                                       "green", "magenta"), xlab = "Time (% of total)", ylab = "Average value for each % of time",
            main = "Result of territory conflict")
    legend("topleft", legend = c("TEnd0", "TEnd1", "TEnd2", "TEnd3", "TEnd4", "TEnd5", "TEnd6"),
           lty = 1, col = c("black", "blue", "brown", "red", "yellow", "green", "magenta"), bg = "white")
    rm(TEnd0, ts0, ts1, ts2, ts3, ts4, ts5, ts6)
    rm(t, tcent)
}

rm(c, cent)


if (TOOBIG) {
    system("mv deadOnes.csv deadOnes.csv.original")
    system("statist --xsample 10 deadOnes.csv.original deadOnes.csv")
}

dead <- read.table("deadOnes.csv", as.is = TRUE, header = TRUE)

if (TOOBIG) {
    system("mv deadOnes.csv.original deadOnes.csv")
}

dm <- subset(dead, sex == 0)
df <- subset(dead, sex == 1)
rm(dead)
gc()

mcent <- round(TMPLENGTH * (dm$time - min(dm$time)) / (max(dm$time) - min(dm$time)))
fcent <- round(TMPLENGTH * (df$time - min(df$time)) / (max(df$time) - min(df$time)))


# Preliminary analysis of friends, enemies, interactions (females)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$nRembr[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$nRembr[mcent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("red", "blue"), main = "Remembered Agents (including dead ones)", xlab = "Time (% of total)", ylab = "Average")
rm(ts1, ts2)

t <- subset(df, (nFFriends > 0), select = c("time", "nFFriends", "nFFriendInt"))
cent <- round(TMPLENGTH * (t$time - min(t$time)) / (max(t$time) - min(t$time)))
r <- t$nFFriendInt / t$nFFriends
rm(t)
for (i in 1:TMPLENGTH) tmp[i] <- mean(r[cent == i])
ts1 <- ts(tmp)
rm(r, cent)

t <- subset(df, (nFEnemies > 0), select = c("time", "nFEnemies", "nFEnemyInt"))
cent <- round(TMPLENGTH * (t$time - min(t$time)) / (max(t$time) - min(t$time)))
r <- t$nFEnemyInt / t$nFEnemies
rm(t)
for (i in 1:TMPLENGTH) tmp[i] <- mean(r[cent == i])
ts2 <- ts(tmp)
rm(r, cent)

t <- subset(df, (nMFriends > 0), select = c("time", "nMFriends", "nMFriendInt"))
cent <- round(TMPLENGTH * (t$time - min(t$time)) / (max(t$time) - min(t$time)))
r <- t$nMFriendInt / t$nMFriends
rm(t)
for (i in 1:TMPLENGTH) tmp[i] <- mean(r[cent == i])
ts3 <- ts(tmp)
rm(r, cent)

t <- subset(df, (nMEnemies > 0), select = c("time", "nMEnemies", "nMEnemyInt"))
cent <- round(TMPLENGTH * (t$time - min(t$time)) / (max(t$time) - min(t$time)))
r <- t$nMEnemyInt / t$nMEnemies
rm(t)
for (i in 1:TMPLENGTH) tmp[i] <- mean(r[cent == i])
ts4 <- ts(tmp)
rm(r, cent)

ts.plot(ts1, ts2, ts3, ts4, lty = c(1, 2, 1, 2), col = c("red", "red", "blue", "blue"), main = "Proportion of n. of interactions to n. of agents (females)", ylab = "Average value for each 1% of time", xlab = "Time (% of total)")
legend("topleft", legend = c("Friends", "Enemies"), lty = c(1, 2), bg = "white")
rm(ts1, ts2, ts3, ts4)


# Preliminary analysis of friends, enemies, interactions (males)
t <- subset(dm, (nFFriends > 0), select = c("time", "nFFriends", "nFFriendInt"))
cent <- round(TMPLENGTH * (t$time - min(t$time)) / (max(t$time) - min(t$time)))
r <- t$nFFriendInt / t$nFFriends
rm(t)
for (i in 1:TMPLENGTH) tmp[i] <- mean(r[cent == i])
ts1 <- ts(tmp)
rm(r, cent)

t <- subset(dm, (nFEnemies > 0), select = c("time", "nFEnemies", "nFEnemyInt"))
cent <- round(TMPLENGTH * (t$time - min(t$time)) / (max(t$time) - min(t$time)))
r <- t$nFEnemyInt / t$nFEnemies
rm(t)
for (i in 1:TMPLENGTH) tmp[i] <- mean(r[cent == i])
ts2 <- ts(tmp)
rm(r, cent)

t <- subset(dm, (nMFriends > 0), select = c("time", "nMFriends", "nMFriendInt"))
cent <- round(TMPLENGTH * (t$time - min(t$time)) / (max(t$time) - min(t$time)))
r <- t$nMFriendInt / t$nMFriends
rm(t)
for (i in 1:TMPLENGTH) tmp[i] <- mean(r[cent == i])
ts3 <- ts(tmp)
rm(r, cent)

t <- subset(dm, (nMEnemies > 0), select = c("time", "nMEnemies", "nMEnemyInt"))
cent <- round(TMPLENGTH * (t$time - min(t$time)) / (max(t$time) - min(t$time)))
r <- t$nMEnemyInt / t$nMEnemies
rm(t)
for (i in 1:TMPLENGTH) tmp[i] <- mean(r[cent == i])
ts4 <- ts(tmp)
rm(r, cent)

ts.plot(ts1, ts2, ts3, ts4, lty = c(1, 2, 1, 2), col = c("red", "red", "blue", "blue"), main = "Proportion of n. of interactions to n. of agents (males)", ylab = "Average value for each 1% of time", xlab = "Time (% of total)")
legend("topleft", legend = c("F. Friends", "F. Enemies", "M. Friends", "M. Enemies"), lty = c(1, 2, 1, 2), col = c("red", "red", "blue", "blue"), bg = "white")
rm(ts1, ts2, ts3, ts4)



for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$nInvt[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$nAcptInvt[mcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$nAlliChng[mcent == i])
ts3 <- ts(tmp)
ts.plot(ts1, ts2, ts3, col = c("blue", "green", "magenta"), main = "Invitations to join an alliance (males)", xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("nInvt", "nAcptInvt", "nAlliChng"), lty = 1, col = c("blue", "green", "magenta"), bg = "white")
rm(ts1, ts2)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$nInvt[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$nAcptInvt[fcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$nAlliChng[fcent == i])
ts3 <- ts(tmp)
ts.plot(ts1, ts2, ts3, col = c("blue", "green", "magenta"), main = "Invitations to join an alliance (females)", xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("nInvt", "nAcptInvt", "nAlliChng"), lty = 1, col = c("blue", "green", "magenta"), bg = "white")
rm(ts1, ts2, ts3)

for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$vengefulness[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$vengStrategy[mcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$vengStr2[mcent == i])
ts3 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$zeroPostvNo[mcent == i])
ts4 <- ts(tmp)
ts.plot(ts1, ts2, ts3, ts4, col = c("red", "blue", "magenta", "green"), main = "males",
        xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("veng.", "vStr.", "vStr2", "zeroNo"),
       col = c("red", "blue", "magenta", "green"),
       lty = 1, bg = "white")
rm(ts1, ts2, ts3, ts4)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$vengefulness[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$vengStrategy[fcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$vengStr2[fcent == i])
ts3 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$zeroPostvNo[fcent == i])
ts4 <- ts(tmp)
ts.plot(ts1, ts2, ts3, ts4, col = c("red", "blue", "magenta", "green"), main = "females",
        xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("veng.", "vStr.", "vStr2", "zeroNo"),
       col = c("red", "blue", "magenta", "green"),
       lty = 1, bg = "white")
rm(ts1, ts2, ts3, ts4)

for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$gratitude[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$gratStrategy[mcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$tmFct[mcent == i])
ts3 <- ts(tmp)
ts.plot(ts1, ts2, ts3, col = c("blue", "green", "red"), main = "males",
        xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("grat", "grStr.", "tmFct"),
       col = c("blue", "green", "red"),
       lty = 1, bty = "n")
rm(ts1, ts2, ts3)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$gratitude[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$gratStrategy[fcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$tmFct[fcent == i])
ts3 <- ts(tmp)
ts.plot(ts1, ts2, ts3, col = c("blue", "green", "red"), main = "females",
        xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("grat", "grStr.", "tmFct"),
       col = c("blue", "green", "red"),
       lty = 1, bty = "n")
rm(ts1, ts2, ts3)


for (i in 1:TMPLENGTH) tmp[i] <- mean(df$maleEnergyImportance[fcent == i])
ts1 = ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$maleAgeImportance[fcent == i])
ts2 = ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$femalePromiscuity[fcent == i])
ts3 = ts(tmp)
ts.plot(ts1, ts2, ts3, col = c("blue", "green", "red"), xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("maleEnergyImportance", "maleAgeImportance", "promiscuity"), lty = 1, col = c("blue", "green", "red"), bg = "white")
rm(ts1, ts2, ts3)

for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$adviceValue[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$adviceValue[fcent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("blue", "red"), main = "Advice value", xlab = "Time (% of total)", ylab = "Average")
rm(ts1, ts2)

for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$vOfFStranger[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$vOfFStranger[fcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$vOfMStranger[mcent == i])
ts3 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$vOfMStranger[fcent == i])
ts4 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$enmityThr[mcent == i])
ts5 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$enmityThr[fcent == i])
ts6 <- ts(tmp)
ts.plot(ts1, ts2, ts3, ts4, ts5, ts6, col = c("blue", "red", "blue", "red", "blue", "red"), lty = c(1, 1, 2, 2, 3, 3),
        main = "Value of Strangers", xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("vOfFStr", "vOfMStr", "enmity"), lty = c(1, 2, 3), bg = "white")
rm(ts1, ts2, ts3, ts4, ts5, ts6)

for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$acceptMoveInvtFromF[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$acceptMoveInvtFromF[fcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$acceptMoveInvtFromM[mcent == i])
ts3 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$acceptMoveInvtFromM[fcent == i])
ts4 <- ts(tmp)
ts.plot(ts1, ts2, ts3, ts4, col = c("blue", "red", "blue", "red"), lty = c(1, 1, 2, 2),
        main = "Accept move invitation?", xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("from f", "from m"), lty = c(1, 2), bg = "white")
rm(ts1, ts2, ts3, ts4)


for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$sameSexSel[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$motherSel[mcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$siblingSel[mcent == i])
ts3 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$friendSel[mcent == i])
ts4 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$otherSexSel[mcent == i])
ts5 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$energySel[mcent == i])
ts6 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$oestrFemSel[mcent == i])
ts7 <- ts(tmp)
ts.plot(ts1, ts2, ts3, ts4, ts5, ts6, ts7, col = c("blue", "red", "green", "orange", "magenta", "grey", "black"),
        main = "male", xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("sameSexSel", "motherSel", "siblingSel", "friendSel", "otherSexSel", "energySel", "oestrFemSel"), col = c("blue", "red", "green", "orange", "magenta", "grey", "black"), lty = 1, bg = "white")
rm(ts1, ts2, ts3, ts4, ts5, ts6, ts7)


for (i in 1:TMPLENGTH) tmp[i] <- mean(df$sameSexSel[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$motherSel[fcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$siblingSel[fcent == i])
ts3 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$friendSel[fcent == i])
ts4 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$otherSexSel[fcent == i])
ts5 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$energySel[fcent == i])
ts6 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$childSel[fcent == i])
ts7 <- ts(tmp)
ts.plot(ts1, ts2, ts3, ts4, ts5, ts6, ts7, col = c("blue", "red", "green", "orange", "magenta", "grey", "black"),
        main = "female", xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("sameSexSel", "motherSel", "siblingSel", "friendSel", "otherSexSel", "energySel", "childSel"),
       lty = 1, col = c("blue", "red", "green", "orange", "magenta", "grey", "black"), bg = "white")
rm(ts1, ts2, ts3, ts4, ts5, ts6, ts7)


for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$migAgeImportance[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$migFriendImportance[mcent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("blue", "red"), xlab = "Time (% of total)", ylab = "Average", main = "males")
legend("topleft", legend = c("migAgeImportance", "migFriendImportance"), col = c("blue", "red"), lty = 1, bg = "white")
rm(ts1, ts2)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$migAgeImportance[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$migFriendImportance[fcent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("blue", "red", "green"), xlab = "Time (% of total)", ylab = "Average", main = "females")
legend("topleft", legend = c("migAgeImportance", "migFriendImportance"), lty = 1, col = c("blue", "red"), bg = "white")
rm(ts1, ts2)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$eFromMom[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$eFromMom[mcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$eFromDinner[fcent == i])
ts3 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$eFromDinner[mcent == i])
ts4 <- ts(tmp)
ts.plot(ts1, ts2, ts3, ts4, col = c("red", "blue", "red", "blue"), lty = c(1, 1, 2, 2), main = "Source of Energy", xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("eFromMom", "eFromDinner"), lty = c(1, 2), bg = "white")
rm(ts1, ts2, ts3, ts4)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$eFromShare[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$eFromShare[mcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$eFromConfl[fcent == i])
ts3 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$eFromConfl[mcent == i])
ts4 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$eFromHunt[fcent == i])
ts5 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$eFromHunt[mcent == i])
ts6 <- ts(tmp)
ts.plot(ts1, ts2, ts3, ts4, ts5, ts6, col = c("red", "blue", "red", "blue", "red", "blue"), lty = c(1, 1, 2, 2, 3, 3), main = "Source of Energy", xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("eFromShare", "eFromConfl", "eFromHunt"), lty = c(1, 2, 3), bg = "white")
rm(ts1, ts2, ts3, ts4, ts5, ts6)


if (TERRITORY) {
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$norm[mcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$metanorm[mcent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$hasShame[mcent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$terriRemStrategy[mcent == i])
    ts4 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, ts4, col = c("blue", "magenta", "red", "green"),
            main = "Norm and Metanorm (m)", xlab = "Time (% of total)", ylab = "Average")
    legend("topleft", legend = c("norm", "metanorm", "shame", "remStr"), col = c("blue", "magenta", "red", "green"),
           lty = 1, bg = "white")
    rm(ts1, ts2, ts3, ts4)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$norm[fcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$metanorm[fcent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$hasShame[fcent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$terriRemStrategy[fcent == i])
    ts4 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, ts4, col = c("blue", "magenta", "red", "green"),
            main = "Norm and Metanorm (f)", xlab = "Time (% of total)", ylab = "Average")
    legend("topleft", legend = c("norm", "metanorm", "shame", "remStr"), col = c("blue", "magenta", "red", "green"),
           lty = 1, bg = "white")
    rm(ts1, ts2, ts3, ts4)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$bravery[mcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$loyalty[mcent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$audacity[mcent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$bravery[fcent == i])
    ts4 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$loyalty[fcent == i])
    ts5 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$audacity[fcent == i])
    ts6 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, ts4, ts5, ts6, col = c("blue", "blue", "blue", "red", "red", "red"),
            lty = c(1, 2, 3, 1, 2, 3), xlab = "Time (% of total)", ylab = "Average", main = "Territory Conflict")
    legend("topleft", legend = c("bravery", "loyalty", "audacity"), lty = c(1, 2, 3), bg = "white")
    rm(ts1, ts2, ts3, ts4, ts5, ts6)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$patrolValue[mcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$patrolValue[fcent == i])
    ts2 <- ts(tmp)
    ts.plot(ts1, ts2, col = c("blue", "red"), main = "Patrol Value",
            xlab = "Time (% of total)", ylab = "Average")

    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$fvalueOfNoCT[mcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$mvalueOfNoCT[mcent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$fvalueOfNoCT[fcent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$mvalueOfNoCT[fcent == i])
    ts4 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, ts4, col = c("blue", "blue", "red", "red"), lty = c(1, 2, 1, 2),
            xlab = "Time (% of total)", ylab = "Average", main = "Value of \"NO\" (territory conflict)")
    legend("topleft", legend = c("males", "females"), lty = c(1, 2), bg = "white")
    rm(ts1, ts2, ts3, ts4)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$xenophTM[mcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$xenophTF[mcent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$xenophTFwK[mcent == i])
    ts3 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, col = c("blue", "red", "green"), main = "Male Xenophobia (towards M, F and FwK)",
            xlab = "Time (% of total)", ylab = "Average")
    rm(ts1, ts2, ts3)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$xenophTM[fcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$xenophTF[fcent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$xenophTFwK[fcent == i])
    ts3 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, col = c("blue", "red", "green"), main = "Female Xenophobia (towards M, F and FwK)",
            xlab = "Time (% of total)", ylab = "Average")
    rm(ts1, ts2, ts3)
}

if (HUNT) {
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$fvalueOfNoH[mcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$mvalueOfNoH[mcent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$fvalueOfNoH[fcent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$mvalueOfNoH[fcent == i])
    ts4 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, ts4, col = c("blue", "blue", "red", "red"), lty = c(1, 2, 1, 2),
            xlab = "Time (% of total)", ylab = "Average", main = "Value of \"NO\" (hunt)")
    legend("topleft", legend = c("males", "females"), lty = c(1, 2), bg = "white")
    rm(ts1, ts2, ts3, ts4)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$huntInterval[mcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$huntInterval[fcent == i])
    ts2 <- ts(tmp)
    ts.plot(ts1, ts2, col = c("blue", "red"), xlab = "Time (% of total)", ylab = "Average", main = "Hunt Interval")
    rm(ts1, ts2)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$huntValue[mcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$huntValue[fcent == i])
    ts2 <- ts(tmp)
    ts.plot(ts1, ts2, col = c("blue", "red"), xlab = "Time (% of total)", ylab = "Average", main = "Hunt Value")
    rm(ts1, ts2)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$fairness[mcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$expFFairness[mcent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$expMFairness[mcent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$fvalueOfNotFair[mcent == i])
    ts4 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$mvalueOfNotFair[mcent == i])
    ts5 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, ts4, ts5, col = c("green", "blue", "red", "blue", "red"), lty = c(1, 1, 1, 2, 2),
            xlab = "Time (% of total)", ylab = "Average", main = "Hunt (males)")
    legend("topleft", legend = c("fairness", "expFF", "expMF", "fvOfNotF", "mvOfNotF"),
           col = c("green", "blue", "red", "blue", "red"), lty = c(1, 1, 1, 2, 2), bg = "white")
    rm(ts1, ts2, ts3, ts4, ts5)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$fairness[fcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$expFFairness[fcent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$expMFairness[fcent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$fvalueOfNotFair[fcent == i])
    ts4 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$mvalueOfNotFair[fcent == i])
    ts5 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, ts4, ts5, col = c("green", "blue", "red", "blue", "red"), lty = c(1, 1, 1, 2, 2),
            xlab = "Time (% of total)", ylab = "Average", main = "Hunt (females)")
    legend("topleft", legend = c("fairness", "expFF", "expMF", "fvOfNotF", "mvOfNotF"),
           col = c("green", "blue", "red", "blue", "red"), lty = c(1, 1, 1, 2, 2), bg = "white")
    rm(ts1, ts2, ts3, ts4, ts5)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$meatValue[mcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$meatValue[fcent == i])
    ts2 <- ts(tmp)
    ts.plot(ts1, ts2, col = c("blue", "red"), xlab = "Time (% of total)", ylab = "Average", main = "Meat Value")
    rm(ts1, ts2)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$maxHuntPatrolSize[mcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$maxHuntPatrolSize[fcent == i])
    ts2 <- ts(tmp)
    ts.plot(ts1, ts2, col = c("blue", "red"), xlab = "Time (% of total)", ylab = "Average", main = "Hunt Patrol Size")
    rm(ts1, ts2)
}

if (FOODSHARE) {
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$generosity[mcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$pity[mcent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$envy[mcent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$meatGenerosity[mcent == i])
    ts4 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, ts4, col = c("blue", "red", "green", "black"), xlab = "Time (% of total)",
            ylab = "Average", main = "males")
    legend("topleft", legend = c("generosity", "pity", "envy", "meatGenerosity"), lty = 1,
           col = c("blue", "red", "green", "black"), bg = "white")
    rm(ts1, ts2, ts3, ts4)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$generosity[fcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$pity[fcent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$envy[fcent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$meatGenerosity[fcent == i])
    ts4 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, ts4, col = c("blue", "red", "green", "black"), xlab = "Time (% of total)",
            ylab = "Average", main = "females")
    legend("topleft", legend = c("generosity", "pity", "envy", "meatGenerosity"), lty = 1,
           col = c("blue", "red", "green", "black"), bg = "white")
    rm(ts1, ts2, ts3, ts4)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$bnvlcTOtherSex[mcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$bnvlcTSameSex[mcent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$bnvlcTMother[mcent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$bnvlcTSibling[mcent == i])
    ts4 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, ts4, col = c("blue", "brown", "red", "green"), main = "males",
            xlab = "Time (% of total)", ylab = "Average")
    legend("topleft", legend = c("bnvlcTOtherSex", "bnvlcTSameSex", "bnvlcTMother", "bnvlcTSibling"),
           col = c("blue", "brown", "red", "green", "magenta"), lty = 1, bg = "white")
    rm(ts1, ts2, ts3, ts4)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$bnvlcTOtherSex[fcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$bnvlcTSameSex[fcent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$bnvlcTMother[fcent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$bnvlcTSibling[fcent == i])
    ts4 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$bnvlcTChild[fcent == i])
    ts5 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, ts4, ts5, col = c("blue", "brown", "red", "green", "magenta"),
            main = "females", xlab = "Time (% of total)", ylab = "Average")
    legend("topleft", legend = c("bnvlcTOtherSex", "bnvlcTSameSex", "bnvlcTMother", "bnvlcTSibling",
                                 "bnvlcTChild"), col = c("blue", "brown", "red", "green", "magenta"), lty = 1, bg = "white")
    rm(ts1, ts2, ts3, ts4, ts5)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$fvalueOfNoSh[mcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$mvalueOfNoSh[mcent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$fvalueOfNoSh[fcent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$mvalueOfNoSh[fcent == i])
    ts4 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, ts4, col = c("blue", "blue", "red", "red"), lty = c(1, 2, 1, 2),
            xlab = "Time (% of total)", ylab = "Average", main = "Value of \"NO\" (food share)")
    legend("topleft", legend = c("females", "males"), lty = c(1, 2), bg = "white")
    rm(ts1, ts2, ts3, ts4)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$askMeatOnly[mcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$askMeatOnly[fcent == i])
    ts2 <- ts(tmp)
    ts.plot(ts1, ts2, col = c("blue", "red"), xlab = "Time (% of total)", ylab = "Average",
            main = "Food: Ask Only Agents with Meat?")
    rm(ts1, ts2)

    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$hiDeficit[fcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(df$lowDeficit[fcent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$hiDeficit[mcent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$lowDeficit[mcent == i])
    ts4 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, ts4, col = c("red", "red", "blue", "blue"), lty = c(1, 2, 1, 2), xlab = "Time (% of total)",
            ylab = "Average", main = "Ask food? Energy deficit values (low and high)")
    legend("topleft", legend = c("high", "low"), lty = c(1, 2), bg = "white")
    rm(ts1, ts2, ts3, ts4)

    f_begStrategy1 <- df$begStrategy
    f_begStrategy1[f_begStrategy1 < 3] <- 0
    f_begStrategy1[f_begStrategy1 > 0] <- 1
    f_begStrategy2 <- df$begStrategy
    f_begStrategy2[f_begStrategy2 == 1 | f_begStrategy2 == 3] <- 0
    f_begStrategy2[f_begStrategy2 > 0] <- 1
    m_begStrategy1 <- dm$begStrategy
    m_begStrategy1[m_begStrategy1 < 3] <- 0
    m_begStrategy1[m_begStrategy1 > 0] <- 1
    m_begStrategy2 <- dm$begStrategy
    m_begStrategy2[m_begStrategy2 == 1 | m_begStrategy2 == 3] <- 0
    m_begStrategy2[m_begStrategy2 > 0] <- 1
    for (i in 1:TMPLENGTH) tmp[i] <- mean(f_begStrategy1[fcent == i])
    ts1 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(m_begStrategy1[mcent == i])
    ts2 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(f_begStrategy2[fcent == i])
    ts3 <- ts(tmp)
    for (i in 1:TMPLENGTH) tmp[i] <- mean(m_begStrategy2[mcent == i])
    ts4 <- ts(tmp)
    ts.plot(ts1, ts2, ts3, ts4, col = c("red", "blue", "red", "blue"), lty = c(1, 1, 2, 2),
            main = "Beg Strategy", xlab = "Time (% of total)", ylab = "Average")
    legend("topleft", legend = c("Strategy 1", "Strategy 2"), lty = c(1, 2), bg = "white")
    rm(ts1, ts2, ts3, ts4, f_begStrategy1, f_begStrategy2, m_begStrategy1, m_begStrategy2)
}

for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$mutationRate[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$mutationRate[fcent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("blue", "red"), main = "mutation rate", xlab = "Time (% of total)", ylab = "Average")
rm(ts1, ts2)


for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$age[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$childhood[mcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$maxAge[mcent == i])
ts3 <- ts(tmp)
ts.plot(ts1, ts2, ts3, col = c("blue", "red", "green"), xlab = "Time (% of total)", ylab = "Average", main = "males")
legend("topleft", legend = c("age", "childhood", "maxAge"), lty = 1, col = c("blue", "red", "green"), bg = "white")
rm(ts1, ts2, ts3)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$age[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$childhood[fcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$maxAge[fcent == i])
ts3 <- ts(tmp)
ts.plot(ts1, ts2, ts3, col = c("blue", "red", "green"), xlab = "Time (% of total)", ylab = "Average", main = "females")
legend("topleft", legend = c("age", "childhood", "maxAge"), lty = 1, col = c("blue", "red", "green"), bg = "white")
rm(ts1, ts2, ts3)


for (i in 1:TMPLENGTH) tmp[i] <- mean(df$bestMaleAge[fcent == i])
ts1 <- ts(tmp)
ts.plot(ts1, xlab = "Time (% of total)", ylab = "Average", main = "bestMaleAge")
rm(ts1)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$kidValueForMale[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$kidValueForMale[mcent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("red", "blue"), xlab = "Time (% of total)", ylab = "Average", main = "kidValueForMale")
rm(ts1, ts2)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$valueOfNoSex[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$valueOfNoSex[mcent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("red", "blue"), xlab = "Time (% of total)", ylab = "Average", main = "Value of NO sex")
rm(ts1, ts2)

for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$nKnownPatches[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$nKnownPatches[fcent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("blue", "red"), main = "nKnownPatches", xlab = "Time (% of total)", ylab = "Average")
rm(ts1, ts2)


t <- subset(dm, nFr > 0, select = c("time", "nFr", "fmni", "fmd"))
tcent <- round(TMPLENGTH * (t$time - min(t$time)) / (max(t$time) - min(t$time)))
for (i in 1:TMPLENGTH) tmp[i] <- mean(t$fmd[tcent == i])
ts1 = ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(t$fmni[tcent == i])
ts2 = ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(t$nFr[tcent == i])
ts3 = ts(tmp)
ts.plot(ts1, ts2, ts3, col = c("blue", "red", "green"), main = "Friends Distance (males)",
        xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("mean fr dist", "mean n. fr. inter.", "nFr"), lty = 1,
       col = c("blue", "red", "green"), bg = "white")
rm(ts1, ts2, ts3, t, tcent)

t <- subset(dm, nEn > 0, select = c("time", "nEn", "emni", "emd"))
tcent <- round(TMPLENGTH * (t$time - min(t$time)) / (max(t$time) - min(t$time)))
for (i in 1:TMPLENGTH) tmp[i] <- mean(t$emd[tcent == i])
ts1 = ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(t$emni[tcent == i])
ts2 = ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(t$nEn[tcent == i])
ts3 = ts(tmp)
ts.plot(ts1, ts2, ts3, col = c("blue", "red", "green"), main = "Enemies Distance (males)", xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("mean en dist", "mean n. en inter.", "nEn"), lty = 1, col = c("blue", "red", "green"), bg = "white")
rm(ts1, ts2, ts3, t, tcent)

t <- subset(df, nFr > 0, select = c("time", "nFr", "fmni", "fmd"))
tcent <- round(TMPLENGTH * (t$time - min(t$time)) / (max(t$time) - min(t$time)))
for (i in 1:TMPLENGTH) tmp[i] <- mean(t$fmd[tcent == i])
ts1 = ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(t$fmni[tcent == i])
ts2 = ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(t$nFr[tcent == i])
ts3 = ts(tmp)
ts.plot(ts1, ts2, ts3, col = c("blue", "red", "green"), main = "Friends Distance (females)",
        xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("mean fr dist", "mean n. fr. inter.", "nFr"), lty = 1,
       col = c("blue", "red", "green"), bg = "white")
rm(ts1, ts2, ts3, t, tcent)

t <- subset(df, nEn > 0, select = c("time", "nEn", "emni", "emd"))
tcent <- round(TMPLENGTH * (t$time - min(t$time)) / (max(t$time) - min(t$time)))
for (i in 1:TMPLENGTH) tmp[i] <- mean(t$emd[tcent == i])
ts1 = ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(t$emni[tcent == i])
ts2 = ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(t$nEn[tcent == i])
ts3 = ts(tmp)
ts.plot(ts1, ts2, ts3, col = c("blue", "red", "green"), main = "Enemies Distance (females)", xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("mean en dist", "mean n. en inter.", "nEn"), lty = 1, col = c("blue", "red", "green"), bg = "white")
rm(ts1, ts2, ts3, t, tcent)


for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$nFFriends[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$nFEnemies[mcent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("blue", "red"), xlab = "Time (% of total)", ylab = "Average", main = "males")
legend("topleft", legend = c("nFFriends", "nFEnemies"), lty = 1, col = c("blue", "red"), bg = "white")
rm(ts1, ts2)

for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$nMFriends[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$nMEnemies[mcent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("blue", "red"), xlab = "Time (% of total)", ylab = "Average", main = "males")
legend("topleft", legend = c("nMFriends", "nMEnemies"), lty = 1, col = c("blue", "red"), bg = "white")
rm(ts1, ts2)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$nFFriends[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$nFEnemies[fcent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("blue", "red"), xlab = "Time (% of total)", ylab = "Average", main = "females")
legend("topleft", legend = c("nFFriends", "nFEnemies"), lty = 1, col = c("blue", "red"), bg = "white")
rm(ts1, ts2)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$nMFriends[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$nMEnemies[fcent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("blue", "red"), xlab = "Time (% of total)", ylab = "Average", main = "females")
legend("topleft", legend = c("nMFriends", "nMEnemies"), lty = 1, col = c("blue", "red"), bg = "white")
rm(ts1, ts2)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$nFFriendInt[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$nFFriendInt[mcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$nFEnemyInt[fcent == i])
ts3 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$nFEnemyInt[mcent == i])
ts4 <- ts(tmp)
ts.plot(ts1, ts2, ts3, ts4, lty = c(1, 1, 2, 2), col = c("red", "blue", "red", "blue"), xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("df$nFFriendInt", "dm$nFFriendInt", "df$nFEnemyInt", "dm$nFEnemyInt"), lty = c(1, 1, 2, 2), col = c("red", "blue", "red", "blue"), bg = "white")
rm(ts1, ts2, ts3, ts4)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$nMFriendInt[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$nMFriendInt[mcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$nMEnemyInt[fcent == i])
ts3 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$nMEnemyInt[mcent == i])
ts4 <- ts(tmp)
ts.plot(ts1, ts2, ts3, ts4, lty = c(1, 1, 2, 2), col = c("red", "blue", "red", "blue"), xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("df$nMFriendInt", "dm$nMFriendInt", "df$nMEnemyInt", "dm$nMEnemyInt"),
       lty = c(1, 1, 2, 2), col = c("red", "blue", "red", "blue"), bg = "white")
rm(ts1, ts2, ts3, ts4)

for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$nFFriendInt[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$nFEnemyInt[mcent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("blue", "red"), main = "Number of interactions with female friends and enemies (males)", xlab = "Time (% of total)", ylab = "Average")
rm(ts1, ts2)

for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$nMFriendInt[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$nMEnemyInt[mcent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("blue", "red"), main = "Number of interactions with male friends and enemies (males)", xlab = "Time (% of total)", ylab = "Average")
rm(ts1, ts2)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$nFFriendInt[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$nFEnemyInt[fcent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("blue", "red"), main = "Number of interactions with female friends and enemies (females)", xlab = "Time (% of total)", ylab = "Average")
rm(ts1, ts2)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$nMFriendInt[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$nMEnemyInt[fcent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("blue", "red"), main = "Number of interactions with male friends and enemies (females)", xlab = "Time (% of total)", ylab = "Average")
rm(ts1, ts2)

for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$minFRecall[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$maxFRecall[mcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$meanFRecall[mcent == i])
ts3 <- ts(tmp)
ts.plot(ts1, ts2, ts3, col = c("red", "blue", "green"), main = "Value of remembrance (min, mean, max) female known agents (males)", xlab = "Time (% of total)", ylab = "Average")
rm(ts1, ts2, ts3)

for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$minMRecall[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$maxMRecall[mcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$meanMRecall[mcent == i])
ts3 <- ts(tmp)
ts.plot(ts1, ts2, ts3, col = c("red", "blue", "green"), main = "Value of remembrance (min, mean, max) of known male agents (males)", xlab = "Time (% of total)", ylab = "Average")
rm(ts1, ts2, ts3)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$minFRecall[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$maxFRecall[fcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$meanFRecall[fcent == i])
ts3 <- ts(tmp)
ts.plot(ts1, ts2, ts3, col = c("red", "blue", "green"), main = "Value of remembrance (min, mean, max) of known female agents (females)", xlab = "Time (% of total)", ylab = "Average")
rm(ts1, ts2, ts3)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$minMRecall[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$maxMRecall[fcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$meanMRecall[fcent == i])
ts3 <- ts(tmp)
ts.plot(ts1, ts2, ts3, col = c("red", "blue", "green"), main = "Value of remembrance (min, aver, max) of known male agents (females)", xlab = "Time (% of total)", ylab = "Average")
rm(ts1, ts2, ts3)

for (i in 1:TMPLENGTH) tmp[i] <- mean(df$acceptMigInv[fcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$fearOfHPatches[fcent == i])
ts2 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(df$fearOfHPWhenHasKid[fcent == i])
ts3 <- ts(tmp)
ts.plot(ts1, ts2, ts3, col = c("blue", "red", "green"), main = "Female Migration",
        xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c( "acptMigInv", "fearHPtc", "fearHPWHasKid"),
       col = c("blue", "red", "green"), lty = 1, bg = "white")
rm(ts1, ts2, ts3)

for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$acceptMigInv[mcent == i])
ts1 <- ts(tmp)
for (i in 1:TMPLENGTH) tmp[i] <- mean(dm$fearOfHPatches[mcent == i])
ts2 <- ts(tmp)
ts.plot(ts1, ts2, col = c("blue", "red"), main = "Male Migration", xlab = "Time (% of total)", ylab = "Average")
legend("topleft", legend = c("acptMigInv", "fearHPtc"), col = c("blue", "red"), lty = 1, bg = "white")
rm(ts1, ts2)

dm$deathCause <- factor(dm$deathCause, levels = c(1, 2 , 3, 4), labels = c("old", "hungry", "prey", "child"))
dm$migPref <- factor(dm$migPref, levels = c(1, 2, 3, 4, 5, 6), labels = c("Tr+Ag", "Ag+Tr", "Tr+Cell", "Ag+Cell", "Cell+Tr", "Cell+Ag"))
dm$migrating <- factor(dm$migrating, levels = c(0, 1, 2, 3, 4, 5), labels = c("No", "BestCell", "Patch", "OtherAg", "Random", "Invitat"))
dm$huntStrategy <- factor(dm$huntStrategy, levels = c(0, 1, 2), labels = c("never", "migrating", "periodically"))


df$deathCause <- factor(df$deathCause, levels = c(1, 2 , 3, 4), labels = c("old", "hungry", "prey", "child"))
df$migPref <- factor(df$migPref, levels = c(1, 2, 3, 4, 5, 6), labels = c("Tr+Ag", "Ag+Tr", "Tr+Cell", "Ag+Cell", "Cell+Tr", "Cell+Ag"))
df$migrating <- factor(df$migrating, levels = c(0, 1, 2, 3, 4, 5), labels = c("No", "BestCell", "Patch", "OtherAg", "Random", "Invitat"))
df$huntStrategy <- factor(df$huntStrategy, levels = c(0, 1, 2), labels = c("never", "migrating", "periodically"))

plot(df$deathCause[fcent == TMPLENGTH], main = "Last 1% of dead agents - females", xlab = "Death cause")
plot(dm$deathCause[mcent == TMPLENGTH], main = "Last 1% of dead agents - males", xlab = "Death cause")
plot(df$migrating[fcent == TMPLENGTH & df$migrating != "No"], main = "Last 1% of dead agents - females", xlab = "Migrating while dying")
plot(dm$migrating[mcent == TMPLENGTH & dm$migrating != "No"], main = "Last 1% of dead agents - males", xlab = "Migrating while dying")

rm(dm, df, mcent, fcent)
gc()


# Last population
lp <- read.table("entirePop.csv", header = TRUE)
f <- subset(lp, (sex == 1))
m <- subset(lp, (sex == 0))
rm(lp)

cat("\n\nLast Population Females\n\n")
summary(f)
cat("\n\nLast Population Males\n\n")
summary(m)


f$gratStrategy <- factor(f$gratStrategy, levels = c(0, 1), labels = c("grateful", "exigent"))
f$vengStrategy <- factor(f$vengStrategy, levels = c(0, 1, 2))
f$vengStr2 <- factor(f$vengStr2, levels = c(0, 1))
f$zeroPostvNo <- factor(f$zeroPostvNo, levels = c(0, 1), labels = c("No", "Yes"))
f$fearOfHPatches <- factor(f$fearOfHPatches, levels = c(0, 1), labels = c("No", "Yes"))
f$fearOfHPWhenHasKid <- factor(f$fearOfHPWhenHasKid, levels = c(0, 1), labels = c("No", "Yes"))
f$acceptMoveInvtFromF <- factor(f$acceptMoveInvtFromF, levels = c(0, 1), labels = c("No", "Yes"))
f$acceptMoveInvtFromM <- factor(f$acceptMoveInvtFromM, levels = c(0, 1), labels = c("No", "Yes"))
f$migrating <- factor(f$migrating, levels = c(0, 1, 2, 3, 4, 5), labels = c("No", "NearCel", "Patch", "FollowOth", "Random", "Invitat"))

m$gratStrategy <- factor(m$gratStrategy, levels = c(0, 1), labels = c("grateful", "exigent"))
m$vengStrategy <- factor(m$vengStrategy, levels = c(0, 1, 2))
m$vengStr2 <- factor(m$vengStr2, levels = c(0, 1))
m$zeroPostvNo <- factor(m$zeroPostvNo, levels = c(0, 1), labels = c("No", "Yes"))
m$fearOfHPatches <- factor(m$fearOfHPatches, levels = c(0, 1), labels = c("No", "Yes"))
m$acceptMoveInvtFromF <- factor(m$acceptMoveInvtFromF, levels = c(0, 1), labels = c ("No", "Yes"))
m$acceptMoveInvtFromM <- factor(m$acceptMoveInvtFromM, levels = c(0, 1), labels = c ("No", "Yes"))
m$migrating <- factor(m$migrating, levels = c(0, 1, 2, 3, 4, 5), labels = c("No", "NearCel", "Patch", "FollowOth", "Random", "Invitat"))


hist(m$mGen)
hist(f$mGen)
hist(m$fGen)
hist(f$fGen)

plot(m$gratStrategy, main = "males - last population", xlab = "gratStrategy")
plot(f$gratStrategy, main = "females - last population", xlab = "gratStrategy")
plot(m$vengStrategy, main = "males - last population", xlab = "vengStrategy")
plot(f$vengStrategy, main = "females - last population", xlab = "vengStrategy")
plot(m$vengStr2, main = "males - last population", xlab = "vengStr2")
plot(f$vengStr2, main = "females - last population", xlab = "vengStr2")
hist(m$gratitude)
hist(f$gratitude)
hist(m$vengefulness)
hist(f$vengefulness)
hist(m$tmFct)
hist(f$tmFct)
plot(m$zeroPostvNo, main = "males - last population", xlab = "zeroPostvNo")
plot(f$zeroPostvNo, main = "females - last population", xlab = "zeroPostvNo")
hist(m$adviceValue)
hist(f$adviceValue)
hist(m$vOfFStranger)
hist(f$vOfFStranger)
hist(m$vOfMStranger)
hist(f$vOfMStranger)
hist(m$enmityThr)
hist(f$enmityThr)

plot(m$acceptMoveInvtFromF, main = "males - last population", xlab = "acceptMoveInvtFromF")
plot(m$acceptMoveInvtFromM, main = "males - last population", xlab = "acceptMoveInvtFromM")
plot(f$acceptMoveInvtFromF, main = "females - last population", xlab = "acceptMoveInvtFromF")
plot(f$acceptMoveInvtFromM, main = "females - last population", xlab = "acceptMoveInvtFromM")
hist(m$sameSexSel)
hist(f$sameSexSel)
hist(f$childSel)
hist(m$motherSel)
hist(f$motherSel)
hist(m$siblingSel)
hist(f$siblingSel)
hist(m$friendSel)
hist(f$friendSel)
hist(m$otherSexSel)
hist(f$otherSexSel)
hist(m$energySel)
hist(f$energySel)
hist(m$oestrFemSel)

hist(f$bestMaleAge)
hist(f$maleEnergyImportance)
hist(f$maleAgeImportance)
hist(f$femalePromiscuity)
hist(m$kidValueForMale)
hist(f$kidValueForMale)
hist(m$valueOfNoSex)
hist(f$valueOfNoSex)

if (FOODSHARE) {
    f_begStrategy1 <- cut(f$begStrategy, 2, labels = c("Averages", "OwnValues"))
    f_begStrategy2 <- f$begStrategy
    f_begStrategy2[f$begStrategy == 3] <- 1
    f_begStrategy2[f_begStrategy2 > 1] <- 2
    f_begStrategy2 <- factor(f_begStrategy2, levels = c(1, 2), labels = c("NoAskZero", "AskForZero"))
    m_begStrategy1 <- cut(m$begStrategy, 2, labels = c("Averages", "OwnValues"))
    m_begStrategy2 <- m$begStrategy
    m_begStrategy2[m$begStrategy == 3] <- 1
    m_begStrategy2[m_begStrategy2 > 1] <- 2
    m_begStrategy2 <- factor(m_begStrategy2, levels = c(1, 2), labels = c("NoAskZero", "AskForZero"))
    plot(m_begStrategy1, main = "males - last population", xlab = "begStrategy1")
    plot(f_begStrategy1, main = "females - last population", xlab = "begStrategy1")
    plot(m_begStrategy2, main = "males - last population", xlab = "begStrategy2")
    plot(f_begStrategy2, main = "females - last population", xlab = "begStrategy2")
    rm(f_begStrategy1, f_begStrategy2, m_begStrategy1, m_begStrategy2)
    f$askMeatOnly <- factor(f$askMeatOnly, levels = c(0, 1), labels = c ("No", "Yes"))
    m$askMeatOnly <- factor(m$askMeatOnly, levels = c(0, 1), labels = c ("No", "Yes"))
    plot(m$askMeatOnly, main = "males - last population", xlab = "askMeatOnly")
    plot(f$askMeatOnly, main = "females - last population", xlab = "askMeatOnly")
    hist(m$bnvlcTOtherSex)
    hist(f$bnvlcTOtherSex)
    hist(m$bnvlcTMother)
    hist(f$bnvlcTMother)
    hist(m$bnvlcTSibling)
    hist(f$bnvlcTSibling)
    hist(f$bnvlcTChild)
    hist(m$bnvlcTSameSex)
    hist(f$bnvlcTSameSex)
    hist(m$generosity)
    hist(f$generosity)
    hist(m$pity)
    hist(f$pity)
    hist(m$envy)
    hist(f$envy)
    hist(m$meatGenerosity)
    hist(f$meatGenerosity)
    hist(m$mvalueOfNoSh)
    hist(m$fvalueOfNoSh)
    hist(f$mvalueOfNoSh)
    hist(f$fvalueOfNoSh)
}
if (TERRITORY) {
    hist(m$bravery)
    hist(f$bravery)
    hist(m$loyalty)
    hist(f$loyalty)
    hist(m$audacity)
    hist(f$audacity)
    hist(m$mvalueOfNoCT)
    hist(m$fvalueOfNoCT)
    hist(f$mvalueOfNoCT)
    hist(f$fvalueOfNoCT)
    hist(m$patrolValue)
    hist(f$patrolValue)
    f$norm <- factor(f$norm, levels = c(0, 1), labels = c ("No", "Yes"))
    f$metanorm <- factor(f$metanorm, levels = c(0, 1), labels = c ("No", "Yes"))
    f$terriRemStrategy <- factor(f$terriRemStrategy, levels = c(0, 1), labels = c("EvenWithoutFight", "OnlyWhenHasFight"))
    f$hasShame <- factor(f$hasShame, levels = c(0, 1), labels = c ("No", "Yes"))
    m$norm <- factor(m$norm, levels = c(0, 1), labels = c ("No", "Yes"))
    m$metanorm <- factor(m$metanorm, levels = c(0, 1), labels = c ("No", "Yes"))
    m$terriRemStrategy <- factor(m$terriRemStrategy, levels = c(0, 1), labels = c("EvenWithoutFight", "OnlyWhenHasFight"))
    m$hasShame <- factor(m$hasShame, levels = c(0, 1), labels = c ("No", "Yes"))
    plot(m$norm, main = "males - last population", xlab = "norm")
    plot(f$norm, main = "females - last population", xlab = "norm")
    plot(m$metanorm, main = "males - last population", xlab = "metanorm")
    plot(f$metanorm, main = "females - last population", xlab = "metanorm")
    plot(m$terriRemStrategy, main = "males - last population", xlab = "terriRemStrategy")
    plot(f$terriRemStrategy, main = "females - last population", xlab = "terriRemStrategy")
    plot(m$hasShame, main = "males - last population", xlab = "hasShame")
    plot(f$hasShame, main = "females - last population", xlab = "hasShame")
}

m$acceptMigInv <- factor(m$acceptMigInv, levels = c(0, 1), labels = c ("No", "Yes"))
f$acceptMigInv <- factor(f$acceptMigInv, levels = c(0, 1), labels = c("No", "Yes"))
f$migPref <- factor(f$migPref, levels = c(1, 2, 3, 4, 5, 6), labels = c("Tr+Ag", "Ag+Tr", "Tr+Cel", "Ag+Cel", "Cel+Tr", "Cel+Ag"))
m$migPref <- factor(m$migPref, levels = c(1, 2, 3, 4, 5, 6), labels = c("Tr+Ag", "Ag+Tr", "Tr+Cel", "Ag+Cel", "Cel+Tr", "Cel+Ag"))
plot(m$acceptMigInv, main = "males - last population", xlab = "acceptMigInvt")
plot(f$acceptMigInv, main = "females - last population", xlab = "acceptMigInvt")
plot(m$migPref, main = "males - last population", xlab = "migPref")
plot(f$migPref, main = "females - last population", xlab = "migPref")
plot(m$migrating, main = "males - last population", xlab = "migrating")
plot(f$migrating, main = "females - last population", xlab = "migrating")
plot(m$fearOfHPatches, main = "males - last population", xlab = "fearOfHPatches")
plot(f$fearOfHPatches, main = "females - last population", xlab = "fearOfHPatches")
plot(f$fearOfHPWhenHasKid, main = "females - last population", xlab = "fearOfHPWhenHasKid")
hist(m$migAgeImportance)
hist(f$migAgeImportance)
hist(m$migFriendImportance)
hist(f$migFriendImportance)
hist(m$hiDeficit)
hist(m$lowDeficit)
hist(f$hiDeficit)
hist(f$lowDeficit)

if (HUNT) {
    m$huntStrategy <- factor(m$huntStrategy, levels = c(0, 1, 2), labels = c("never", "migrating", "periodically"))
    f$huntStrategy <- factor(f$huntStrategy, levels = c(0, 1, 2), labels = c("never", "migrating", "periodically"))
    plot(m$huntStrategy, main = "males - last population", xlab = "huntStrategy")
    plot(f$huntStrategy, main = "females - last population", xlab = "huntStrategy")
    hist(m$huntInterval)
    hist(f$huntInterval)
    hist(m$huntValue)
    hist(f$huntValue)
    hist(m$meatValue)
    hist(f$meatValue)
    hist(m$maxHuntPatrolSize)
    hist(f$maxHuntPatrolSize)
    hist(m$fvalueOfNoH)
    hist(f$fvalueOfNoH)
    hist(m$mvalueOfNoH)
    hist(f$mvalueOfNoH)
    hist(m$fairness)
    hist(f$fairness)
    hist(m$expFFairness)
    hist(f$expFFairness)
    hist(m$expMFairness)
    hist(f$expMFairness)
    hist(m$fvalueOfNotFair)
    hist(f$fvalueOfNotFair)
    hist(m$mvalueOfNotFair)
    hist(f$mvalueOfNotFair)
}

hist(m$energy)
hist(f$energy)
hist(m$mutationRate)
hist(f$mutationRate)
hist(m$age)
hist(f$age)
hist(m$childhood)
hist(f$childhood)
hist(m$maxAge)
hist(f$maxAge)
hist(m$nLiveChld)
hist(f$nLiveChld)

rm(m, f, tmp, TMPLENGTH, i, FOODSHARE, HUNT, TERRITORY)
gc()

cat("End of analysis of last population\n")
ls()

# if really too big, create a csv file with only (ord == 1 and recall > 0)
# and analyse with analyse_HugeMem.R.
# mv agRemembrances.csv agRemembrances.csv.orig
# head -n 2 agRemembrances.csv.orig > agRemembrances.csv
# awk '{ if ($9 == 1 && $20 > 0) print $0 }' agRemembrances.csv.orig >> agRemembrances.csv
# Problem: remembered and remembering agents will not be exactly the same.
# awk '{if ( $9 == 1 ) print $1 }' agRemembrances.csv.orig >> agRemembNames
# sort -u agRemembNames > agRemembNames.csv

b <- read.table("agRemembrances.csv", header = TRUE)

b$event <- factor(b$event, levels = c(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20),
                  labels = c( "I received food", "I gave food", "Reading my mother's memory", "My gratitude to my mother", "My child is adult now", "I received a child", "I gave a child", "No in territory confl. ivt", "Patrol ally", "Patrol enemy", "No in hunt invitation", "Hunt partner", "No in sex proposal", "I was in a kid's mother mem", "No in food share", "I said NO in sex proposal", "I punished", "I was punished", "Not fair in meat distrib.", "Bold request for food"))
summary(b$event)
b$event <- factor(b$event)

b$kind <- vector(length = length(b$recall), mode = "numeric")
b$kind[b$recall < b$enmityThr] <- 1			# Enemy
b$kind[b$recall >= b$enmityThr & b$enmityThr < 0] <- 2	# Refractory
b$kind[b$recall == 0] <- 3				# Neutral
b$kind[b$recall > 0] <- 4				# Friend

b$kind.f <- factor(b$kind, levels = c(1, 2, 3, 4), labels = c("Enemy", "Refractory", "Neutral", "Friend"))
b$kinship.f <- factor(b$kinship, levels = c(1, 2, 3, 4), labels = c("Stranger", "Mother", "Sibling", "Child"))

recallNoZero <- (b$recall != 0)
cat("\n =========================================================================== ")
cat("\nWhat events are more important for having remembrances different from zero?\n")
crosstab(b$event, recallNoZero, expected = TRUE)
summary(lm(b$recall ~ b$event))
rm(recallNoZero)
gc()

#summary(b)

if (max(b$x1) > max(b$x2)) maxX <- max(b$x1) else maxX <- max(b$x2)
if (max(b$y1) > max(b$y2)) maxY <- max(b$y1) else maxY <- max(b$y2)

bf <- subset(b, ord == 1 & recall > 0)
be <- subset(b, ord == 1 & recall < 0)
xf <- tapply(bf$x2, bf$agent, mean, simplify = TRUE)
yf <- tapply(bf$y2, bf$agent, mean, simplify = TRUE)
xe <- tapply(be$x2, be$agent, mean, simplify = TRUE)
ye <- tapply(be$y2, be$agent, mean, simplify = TRUE)
plot(xf, yf, main = "Map of centers of positive and negative memories\n(last population)", col = "blue", xlab = "x", ylab = "y", xlim = c(0, maxX), ylim = c(0, maxY))
points(xe, ye, col = "red")
rm(bf, be, xf, yf, xe, ye)


# Building matrix for network analysis
# Recall subset: each line corresponds to one recall value
b1 <- subset(b, ord == 1, select = c("agent", "myID", "mySID", "sex1", "x1", "y1", "n", "ns", "other", "otherID", "otherSID", "first", "nInter", "sex2", "kinship", "kinship.f", "dist", "recall", "enmityThr", "nSF", "nSE", "nSK", "kind", "kind.f"))


rm(b)

cat("Location: deleted b\n")
ls()

summary(b1)

nAgents <- max(b1$myID)
if (nAgents > 2000) {
    save(b1, file = "b1.RData")
    quit(save = "no")
}

if (nAgents > 100)
    TOOBIG <- 1

# Are the agents being able to form groups of friends?
boxplot(dist ~ sex1 + sex2, col = c(6, 7), data = b1, notch = TRUE, varwidth = TRUE,
        outline = F, ylab = "Distance", xlab = "Sex of agents",
        main = "Distance of remembered agents")

boxplot(dist ~ kinship.f + sex1, col = c(11, 6, 9, 5), data = b1, notch = TRUE,
        outline = F, varwidth = TRUE, xlab = "Type of relationship", ylab = "Distance",
        main = "Distance of remembered agents")

boxplot(dist ~ kind.f + sex1, col = c(6, 9, 5), data = b1, notch = TRUE, varwidth = TRUE,
        outline = F, xlab = "Type of remembrance", ylab = "Distance",
        main = "Distance of remembered agents")

cat("\n\n ================================================================= \n")
summary(lm(dist ~ recall + kinship.f + enmityThr + kind.f + first + sex2, data = b1, sex1 == "f"))
cat("\n\n ================================================================= \n")
summary(lm(dist ~ recall + kinship.f + enmityThr + kind.f + first + sex2, data = b1, sex1 == "m"))
cat("\n\n ================================================================= \n")

library(igraph)


createEdgeList <- function(x) {
    len <- length(x$myID)
    if (len > 0)
    {
        el <- matrix(ncol = 2, nrow = len)
        for (i in 1:len)
        {
            el[i, 1] <- x$myID[i]
            el[i, 2] <- x$otherID[i]
        }
        el
    }
}


# Creating subsets of b1 for all agents, regardless of sex
b1.fr <- subset(b1, kind == 4, select = c("myID", "otherID"))
if (!TOOBIG) {
    b1.nt <- subset(b1, kind == 3, select = c("myID", "otherID"))
    b1.rf <- subset(b1, kind == 2, select = c("myID", "otherID"))
    b1.en <- subset(b1, kind == 1, select = c("myID", "otherID"))
    b1.cf <- subset(b1,   nSF > 0, select = c("myID", "otherID", "nSF"))
    b1.cn <- subset(b1,   nSK > 0, select = c("myID", "otherID", "nSK"))
    b1.ce <- subset(b1,   nSE > 0, select = c("myID", "otherID", "nSE"))
}

# Creating an edgelist for each subset:
el.fr <- createEdgeList(b1.fr)
rm(b1.fr)
if (!TOOBIG) {
    el.nt <- createEdgeList(b1.nt)
    el.rf <- createEdgeList(b1.rf)
    el.en <- createEdgeList(b1.en)
    el.cf <- createEdgeList(b1.cf)
    el.cn <- createEdgeList(b1.cn)
    el.ce <- createEdgeList(b1.ce)
    rm(b1.nt, b1.rf, b1.en, b1.cf, b1.cn, b1.ce)
}

plot(graph.data.frame(el.fr), vertex.size = 5, edge.arrow.size = 0.1, layout = layout_with_fr,
     vertex.label = NA, main = "Friends")

sink()
dev.off()
warnings()
quit(save = "no")

# The code below uses the package statnet which can no longer be installed from CRAN

if (!TOOBIG) {
    # Fill a matrix with connectedness and density values
    addLine <- function(m, r, e, n)
    {
        if (length(e) > 0) {
            nt <- network.initialize(n)
            add.edges(nt, e[, 1], e[, 2])
            m[r, 1] <- connectedness(nt)
            m[r, 2] <- gden(nt)
        }
        m
    }

    # Calculating connectedness and density of networks
    m <- matrix(nrow = 7, ncol = 2)
    rownames(m) <- c("Friends", "Neutrals", "Refractor",
                     "Enemies", "Common Friends", "Common Neutrals", "Common Enemies")
    colnames(m) <- c("Connectedness", "Density")

    m <- addLine(m, 1, el.fr, nAgents)
    m <- addLine(m, 2, el.nt, nAgents)
    m <- addLine(m, 3, el.rf, nAgents)
    m <- addLine(m, 4, el.en, nAgents)
    m <- addLine(m, 5, el.cf, nAgents)
    m <- addLine(m, 6, el.cn, nAgents)
    m <- addLine(m, 7, el.ce, nAgents)

    cat("Connectedness and density of networks (entire population):\n")
    m
}

# Building edges attributes (all agents)
b1len <- length(b1$myID)
agKinship <- matrix(nrow = nAgents, ncol = nAgents)
for (i in 1:b1len) {
    agKinship[b1$myID[i], b1$otherID[i]] <- b1$kinship[i]
}
rm(b1len)

# Building vertex attributes (all agents)
agSex <- vector(length =  nAgents, mode = "numeric")
agNames <- vector(length =  nAgents, mode = "character")
agXY <- matrix(nrow =  nAgents, ncol = 2)
b2 <- subset(b1, n == 1, select = c("agent", "myID", "sex1", "x1", "y1"))
tmpNames <- b2$agent
nni <- length(tmpNames)
for (i in 1:nni) {
    idx <- grep(tmpNames[i], b2$agent)
    id <- b2$myID[idx]
    agSex[id] <- as.numeric(b2$sex1[idx])
    if (agSex[id] == 1) agSex[id] <- 6  else agSex[id] <- 7 # blue | red
    agXY[id, 1] <- b2$x1[idx]
    agXY[id, 2] <- b2$y1[idx]
    agNames[id] <- as.character(b2$agent[idx])
}
rm(b2, nni)

# Network plots for all agents relationships + calculus of betweenness, degree and prestige
net <- network.initialize(nAgents)
add.edges(net, el.fr[, 1], el.fr[, 2])
plot(net, vertex.col = agSex, edge.col = agKinship, main = "Friends")
legend("topleft", legend = c("Stranger", "Mother", "Sibling", "Child"), col = 1:4, lty = 1, bty = "n")
fr.bt <- betweenness(net)
fr.deg <- degree(net)
fr.prest <- prestige(net)
rm(el.fr)

if (!TOOBIG) {
    add.edges(net, el.nt[, 1], el.nt[, 2])
    plot(net, vertex.col = agSex, edge.col = agKinship, main = "Friends & neutrals")
    legend("topleft", legend = c("Stranger", "Mother", "Sibling", "Child"), col = 1:4, lty = 1, bty = "n")
    frnt.bt <- betweenness(net)
    frnt.deg <- degree(net)
    frnt.prest <- prestige(net)

    add.edges(net, el.rf[, 1], el.rf[, 2])
    plot(net, vertex.col = agSex, edge.col = agKinship, main = "Friends, neutrals, and refractories")
    legend("topleft", legend = c("Stranger", "Mother", "Sibling", "Child"), col = 1:4, lty = 1, bty = "n")
    frntrf.bt <- betweenness(net)
    frntrf.deg <- degree(net)
    frntrf.prest <- prestige(net)

    net <- network.initialize(nAgents)
    add.edges(net, el.en[, 1], el.en[, 2])
    plot(net, vertex.col = agSex, edge.col = agKinship, main = "Enemies")
    legend("topleft", legend = c("Stranger", "Mother", "Sibling", "Child"), col = 1:4, lty = 1, bty = "n")
    en.bt <- betweenness(net)
    en.deg <- degree(net)
    en.prest <- prestige(net)

    net <- network.initialize(nAgents)
    add.edges(net, el.cf[, 1], el.cf[, 2])
    plot(net, vertex.col = agSex, edge.col = agKinship, main = "Common friends")
    legend("topleft", legend = c("Stranger", "Mother", "Sibling", "Child"), col = 1:4, lty = 1, bty = "n")
    cf.bt <- betweenness(net)
    cf.deg <- degree(net)
    cf.prest <- prestige(net)

    net <- network.initialize(nAgents)
    add.edges(net, el.ce[, 1], el.ce[, 2])
    plot(net, vertex.col = agSex, edge.col = agKinship, main = "Common enemies")
    legend("topleft", legend = c("Stranger", "Mother", "Sibling", "Child"), col = 1:4, lty = 1, bty = "n")
    ce.bt <- betweenness(net)
    ce.deg <- degree(net)
    ce.prest <- prestige(net)

    net <- network.initialize(nAgents)
    add.edges(net, el.cn[, 1], el.cn[, 2])
    plot(net, vertex.col = agSex, edge.col = agKinship, main = "Common neutrals")
    legend("topleft", legend = c("Stranger", "Mother", "Sibling", "Child"), col = 1:4, lty = 1, bty = "n")
    cn.bt <- betweenness(net)
    cn.deg <- degree(net)
    cn.prest <- prestige(net)
    rm(el.nt, el.rf, el.en, el.cf, el.cn, el.ce)
}
rm(agSex, agKinship)

if (!TOOBIG) {
    tmp.df <- data.frame(agNames, fr.bt, fr.deg, fr.prest, frnt.bt, frnt.deg,
                         frnt.prest, frntrf.bt, frntrf.deg, frntrf.prest, en.bt, en.deg, en.prest,
                         cf.bt, cf.deg, cf.prest, cn.bt, cn.deg, cn.prest, ce.bt, ce.deg, ce.prest)
    names(tmp.df) <- c("name", "fr.bt", "fr.deg", "fr.prest", "frnt.bt", "frnt.deg",
                       "frnt.prest", "frntrf.bt", "frntrf.deg", "frntrf.prest", "en.bt", "en.deg", "en.prest",
                       "cf.bt", "cf.deg", "cf.prest", "cn.bt", "cn.deg", "cn.prest", "ce.bt", "ce.deg", "ce.prest")
    lp <- read.table("entirePop.csv", header = TRUE)
    lp$name <- factor(lp$name) # The merge works better with factor
    lp2 <- merge(lp, tmp)
    lp2$sex <- factor(lp2$sex, levels = c(0, 1), labels = c("male", "female"))

    summary(lm(fr.bt ~ sex, lp2))
    summary(lm(fr.deg ~ sex, lp2))
    summary(lm(fr.prest ~ sex, lp2))
    summary(lm(frnt.bt ~ sex, lp2))
    summary(lm(frnt.deg ~ sex, lp2))
    summary(lm(frnt.prest ~ sex, lp2))
    summary(lm(frntrf.bt ~ sex, lp2))
    summary(lm(frntrf.deg ~ sex, lp2))
    summary(lm(frntrf.prest ~ sex, lp2))
    summary(lm(en.bt ~ sex, lp2))
    summary(lm(en.deg ~ sex, lp2))
    summary(lm(en.prest ~ sex, lp2))
    summary(lm(cf.bt ~ sex, lp2))
    summary(lm(cf.deg ~ sex, lp2))
    summary(lm(cf.prest ~ sex, lp2))
    summary(lm(cn.bt ~ sex, lp2))
    summary(lm(cn.deg ~ sex, lp2))
    summary(lm(cn.prest ~ sex, lp2))
    summary(lm(ce.bt ~ sex, lp2))
    summary(lm(ce.deg ~ sex, lp2))
    summary(lm(ce.prest ~ sex, lp2))

    rm(lp2)
}

# Creating subsets of b1 for agents of opposite sex
b1.mf.fr <- subset(b1, sex1 == 'm' & sex2 == 'f' & kind == 4, select = c("myID", "otherID"))
b1.fm.fr <- subset(b1, sex1 == 'f' & sex2 == 'm' & kind == 4, select = c("myID", "otherID"))
b1.mf.cf <- subset(b1, sex1 == 'm' & sex2 == 'f' & nSF > 0, select = c("myID", "otherID", "nSF"))
b1.fm.cf <- subset(b1, sex1 == 'f' & sex2 == 'm' & nSF > 0, select = c("myID", "otherID", "nSF"))

if (!TOOBIG) {
    b1.mf.nt <- subset(b1, sex1 == 'm' & sex2 == 'f' & kind == 3, select = c("myID", "otherID"))
    b1.mf.rf <- subset(b1, sex1 == 'm' & sex2 == 'f' & kind == 2, select = c("myID", "otherID"))
    b1.mf.en <- subset(b1, sex1 == 'm' & sex2 == 'f' & kind == 1, select = c("myID", "otherID"))
    b1.fm.nt <- subset(b1, sex1 == 'f' & sex2 == 'm' & kind == 3, select = c("myID", "otherID"))
    b1.fm.rf <- subset(b1, sex1 == 'f' & sex2 == 'm' & kind == 2, select = c("myID", "otherID"))
    b1.fm.en <- subset(b1, sex1 == 'f' & sex2 == 'm' & kind == 1, select = c("myID", "otherID"))
    b1.mf.cn <- subset(b1, sex1 == 'm' & sex2 == 'f' & nSK > 0, select = c("myID", "otherID", "nSK"))
    b1.mf.ce <- subset(b1, sex1 == 'm' & sex2 == 'f' & nSE > 0, select = c("myID", "otherID", "nSE"))
    b1.fm.cn <- subset(b1, sex1 == 'f' & sex2 == 'm' & nSK > 0, select = c("myID", "otherID", "nSK"))
    b1.fm.ce <- subset(b1, sex1 == 'f' & sex2 == 'm' & nSE > 0, select = c("myID", "otherID", "nSE"))
}

# Creating an edgelist for each subset:
el.mf.fr <- createEdgeList(b1.mf.fr)
el.fm.fr <- createEdgeList(b1.fm.fr)
el.mf.cf <- createEdgeList(b1.mf.cf)
el.fm.cf <- createEdgeList(b1.fm.cf)
rm(b1.mf.fr, b1.fm.fr, b1.mf.cf, b1.fm.cf)

if (!TOOBIG) {
    el.mf.nt <- createEdgeList(b1.mf.nt)
    el.mf.rf <- createEdgeList(b1.mf.rf)
    el.mf.en <- createEdgeList(b1.mf.en)
    el.fm.nt <- createEdgeList(b1.fm.nt)
    el.fm.rf <- createEdgeList(b1.fm.rf)
    el.fm.en <- createEdgeList(b1.fm.en)
    el.mf.cn <- createEdgeList(b1.mf.cn)
    el.mf.ce <- createEdgeList(b1.mf.ce)
    el.fm.cn <- createEdgeList(b1.fm.cn)
    el.fm.ce <- createEdgeList(b1.fm.ce)
    rm(b1.mf.nt, b1.mf.rf, b1.mf.en, b1.fm.nt, b1.fm.rf,
       b1.fm.en, b1.mf.cn, b1.mf.ce, b1.fm.cn, b1.fm.ce)
}


# Creating subsets of b1 for same sex agents
b1.mm.fr <- subset(b1, sex1 == 'm' & sex2 == 'm' & kind == 4, select = c("mySID", "otherSID"))
b1.ff.fr <- subset(b1, sex1 == 'f' & sex2 == 'f' & kind == 4, select = c("mySID", "otherSID"))
b1.mm.cf <- subset(b1, sex1 == 'm' & sex2 == 'm' & nSF > 0, select = c("mySID", "otherSID", "nSF"))
b1.ff.cf <- subset(b1, sex1 == 'f' & sex2 == 'f' & nSF > 0, select = c("mySID", "otherSID", "nSF"))

if (!TOOBIG) {
    b1.mm.nt <- subset(b1, sex1 == 'm' & sex2 == 'm' & kind == 3, select = c("mySID", "otherSID"))
    b1.mm.rf <- subset(b1, sex1 == 'm' & sex2 == 'm' & kind == 2, select = c("mySID", "otherSID"))
    b1.mm.en <- subset(b1, sex1 == 'm' & sex2 == 'm' & kind == 1, select = c("mySID", "otherSID"))
    b1.ff.nt <- subset(b1, sex1 == 'f' & sex2 == 'f' & kind == 3, select = c("mySID", "otherSID"))
    b1.ff.rf <- subset(b1, sex1 == 'f' & sex2 == 'f' & kind == 2, select = c("mySID", "otherSID"))
    b1.ff.en <- subset(b1, sex1 == 'f' & sex2 == 'f' & kind == 1, select = c("mySID", "otherSID"))
    b1.mm.cn <- subset(b1, sex1 == 'm' & sex2 == 'm' & nSK > 0, select = c("mySID", "otherSID", "nSK"))
    b1.mm.ce <- subset(b1, sex1 == 'm' & sex2 == 'm' & nSE > 0, select = c("mySID", "otherSID", "nSE"))
    b1.ff.cn <- subset(b1, sex1 == 'f' & sex2 == 'f' & nSK > 0, select = c("mySID", "otherSID", "nSK"))
    b1.ff.ce <- subset(b1, sex1 == 'f' & sex2 == 'f' & nSE > 0, select = c("mySID", "otherSID", "nSE"))
}


# Creating an edgelist for each subset of same sex agents:
createEdgeList <- function(x) {
    len <- length(x$mySID)
    if (len > 0)
    {
        el <- matrix(ncol = 2, nrow = len)
        for (i in 1:len)
        {
            el[i, 1] <- x$mySID[i]
            el[i, 2] <- x$otherSID[i]
        }
        el
    }
}

el.mm.fr <- createEdgeList(b1.mm.fr)
el.ff.fr <- createEdgeList(b1.ff.fr)
el.mm.cf <- createEdgeList(b1.mm.cf)
el.ff.cf <- createEdgeList(b1.ff.cf)

if (!TOOBIG) {
    el.mm.nt <- createEdgeList(b1.mm.nt)
    el.mm.rf <- createEdgeList(b1.mm.rf)
    el.mm.en <- createEdgeList(b1.mm.en)
    el.ff.nt <- createEdgeList(b1.ff.nt)
    el.ff.rf <- createEdgeList(b1.ff.rf)
    el.ff.en <- createEdgeList(b1.ff.en)
    el.mm.cn <- createEdgeList(b1.mm.cn)
    el.mm.ce <- createEdgeList(b1.mm.ce)
    el.ff.cn <- createEdgeList(b1.ff.cn)
    el.ff.ce <- createEdgeList(b1.ff.ce)
    rm(b1.mm.nt, b1.mm.rf, b1.mm.en, b1.ff.nt,
       b1.ff.rf, b1.ff.en, b1.mm.cn, b1.mm.ce, b1.ff.cn, b1.ff.ce)
}

rm(createEdgeList, b1.mm.fr, b1.ff.fr, b1.mm.cf, b1.ff.cf)

if (!TOOBIG) {
    # Calculating connectedness and density of networks
    m <- matrix(nrow = 16, ncol = 2)
    rownames(m) <- c("MM Friends", "MM Neutrals", "MM Refractor",
                     "MM Enemies", "FF Friends", "FF Neutrals", "FF Refractor",
                     "FF Enemies", "MF Friends", "MF Neutrals", "MF Refractor", "MF Enemies",
                     "FM Friends", "FM Neutrals", "FM Refractor", "FM Enemies")
    colnames(m) <- c("Connectedness", "Density")

    m <- addLine(m,  1, el.mm.fr, nMMAg)
    m <- addLine(m,  2, el.mm.nt, nMMAg)
    m <- addLine(m,  3, el.mm.rf, nMMAg)
    m <- addLine(m,  4, el.mm.en, nMMAg)
    m <- addLine(m,  5, el.ff.fr, nFFAg)
    m <- addLine(m,  6, el.ff.nt, nFFAg)
    m <- addLine(m,  7, el.ff.rf, nFFAg)
    m <- addLine(m,  8, el.ff.en, nFFAg)
    m <- addLine(m,  9, el.mf.fr, nAgents)
    m <- addLine(m, 10, el.mf.nt, nAgents)
    m <- addLine(m, 11, el.mf.rf, nAgents)
    m <- addLine(m, 12, el.mf.en, nAgents)
    m <- addLine(m, 13, el.fm.fr, nAgents)
    m <- addLine(m, 14, el.fm.nt, nAgents)
    m <- addLine(m, 15, el.fm.rf, nAgents)
    m <- addLine(m, 16, el.fm.en, nAgents)

    m

    rm(m, addLine)
}

# Building edges attributes (males)
b2 <- subset(b1, sex1 == 'm' & sex2 == 'm', select = c("kinship", "mySID", "otherSID"))
nMMAg <- max(b2$mySID)
b2len <- length(b2$mySID)
mmKinship <- matrix(nrow = nMMAg, ncol = nMMAg)
for (i in 1:b2len) {
    mmKinship[b2$mySID[i], b2$otherSID[i]] <- b2$kinship[i]
}
rm(b2, b2len)

# Network plots for male-male relationships + calculus of betweenness, degree and prestige
net <- network.initialize(nMMAg)
add.edges(net, el.mm.fr[, 1], el.mm.fr[, 2])
plot(net, vertex.col = 7, edge.col = mmKinship, main = "M-M: friends")
legend("topleft", legend = c("Stranger", "Mother", "Sibling"), col = 1:3, lty = 1, bty = "n")
mm.fr.bt <- betweenness(net)
mm.fr.deg <- degree(net)
mm.fr.prest <- prestige(net)

if (!TOOBIG) {
    add.edges(net, el.mm.nt[, 1], el.mm.nt[, 2])
    plot(net, vertex.col = 7, edge.col = mmKinship, main = "M-M: friends & neutrals")
    legend("topleft", legend = c("Stranger", "Mother", "Sibling"), col = 1:3, lty = 1, bty = "n")
    mm.frnt.bt <- betweenness(net)
    mm.frnt.deg <- degree(net)
    mm.frnt.prest <- prestige(net)

    add.edges(net, el.mm.rf[, 1], el.mm.rf[, 2])
    plot(net, vertex.col = 7, edge.col = mmKinship, main = "M-M: friends, neutrals, and refractories")
    legend("topleft", legend = c("Stranger", "Mother", "Sibling"), col = 1:3, lty = 1, bty = "n")
    mm.frntrf.bt <- betweenness(net)
    mm.frntrf.deg <- degree(net)
    mm.frntrf.prest <- prestige(net)

    net <- network.initialize(nMMAg)
    add.edges(net, el.mm.en[, 1], el.mm.en[, 2])
    plot(net, vertex.col = 7, edge.col = mmKinship, main = "M-M: enemies")
    legend("topleft", legend = c("Stranger", "Mother", "Sibling"), col = 1:3, lty = 1, bty = "n")
    mm.en.bt <- betweenness(net)
    mm.en.deg <- degree(net)
    mm.en.prest <- prestige(net)
}

# Building vertex attributes (males)
b2 <- subset(b1, sex1 == 'f', select = c("agent", "n", "mySID", "x1", "y1"))
b2$agent <- factor(b2$agent)
tmpNames <- levels(b2$agent)
b2 <- subset(b2, n == 1)
mmNames <- vector(length = nMMAg, mode = "character")
mmXY <- matrix(nrow =  nMMAg, ncol = 2)
n <- length(tmpNames)
for (i in 1:n) {
    idx <- grep(tmpNames[i], b2$agent)
    if (length(idx) > 1) print(c(i, idx))
    id <- b2$mySID[idx]
    mmXY[id, 1] <- b2$x1[idx]
    mmXY[id, 2] <- b2$y1[idx]
    mmNames[id] <- as.character(b2$agent[idx])
}
rm(b2, mmKinship, tmpNames, n, i, id, idx)

if (!TOOBIG) {
    mm.df <- data.frame(mmNames, mm.fr.bt, mm.fr.deg, mm.fr.prest, mm.frnt.bt,
                        mm.frnt.deg, mm.frnt.prest, mm.frntrf.bt, mm.frntrf.deg, mm.frntrf.prest,
                        mm.en.bt, mm.en.deg, mm.en.prest)

    rm(mmNames, mm.fr.bt, mm.fr.deg, mm.fr.prest, mm.frnt.bt, mm.frnt.deg,
       mm.frnt.prest, mm.frntrf.bt, mm.frntrf.deg, mm.frntrf.prest, mm.en.bt,
       mm.en.deg, mm.en.prest)
}

rm(mmNames)

# Building edges attributes (females)
b2 <- subset(b1, sex1 == 'f' & sex2 == 'f', select = c("kinship", "mySID", "otherSID"))
nFFAg <- max(b2$mySID)
b2len <- length(b2$mySID)
ffKinship <- matrix(nrow = nFFAg, ncol = nFFAg)
for (i in 1:b2len) {
    ffKinship[b2$mySID[i], b2$otherSID[i]] <- b2$kinship[i]
}
rm(b2, b2len)

# Network plots for female-female relationships + calculus of betweenness, degree and prestige
net <- network.initialize(nFFAg)
add.edges(net, el.ff.fr[, 1], el.ff.fr[, 2])
plot(net, vertex.col = 6, edge.col = ffKinship, main = "F-F: friends")
legend("topleft", legend = c("Stranger", "Mother", "Sibling", "Child"), col = 1:4, lty = 1, bty = "n")
ff.fr.bt <- betweenness(net)
ff.fr.deg <- degree(net)
ff.fr.prest <- prestige(net)

if (!TOOBIG) {
    add.edges(net, el.ff.nt[, 1], el.ff.nt[, 2])
    plot(net, vertex.col = 6, edge.col = ffKinship, main = "F-F: friends & neutrals")
    legend("topleft", legend = c("Stranger", "Mother", "Sibling", "Child"), col = 1:4, lty = 1, bty = "n")
    ff.frnt.bt <- betweenness(net)
    ff.frnt.deg <- degree(net)
    ff.frnt.prest <- prestige(net)

    add.edges(net, el.ff.rf[, 1], el.ff.rf[, 2])
    plot(net, vertex.col = 6, edge.col = ffKinship, main = "F-F: friends, neutrals, and refractories")
    legend("topleft", legend = c("Stranger", "Mother", "Sibling", "Child"), col = 1:4, lty = 1, bty = "n")
    ff.frntrf.bt <- betweenness(net)
    ff.frntrf.deg <- degree(net)
    ff.frntrf.prest <- prestige(net)

    net <- network.initialize(nFFAg)
    add.edges(net, el.ff.en[, 1], el.ff.en[, 2])
    plot(net, vertex.col = 6, edge.col = ffKinship, main = "F-F: enemies")
    legend("topleft", legend = c("Stranger", "Mother", "Sibling", "Child"), col = 1:4, lty = 1, bty = "n")
    ff.en.bt <- betweenness(net)
    ff.en.deg <- degree(net)
    ff.en.prest <- prestige(net)
    rm(el.fm.ce, el.fm.cn, el.fm.en, el.fm.nt, el.fm.rf)
    rm(el.mf.ce, el.mf.cn, el.mf.en, el.mf.nt, el.mf.rf)
    rm(el.ff.ce, el.ff.cn, el.ff.en, el.ff.nt, el.ff.rf)
    rm(el.mm.ce, el.mm.cn, el.mm.en, el.mm.nt, el.mm.rf)
}

rm(ffKinship)
rm(el.fm.cf, el.fm.fr)
rm(el.mf.cf, el.mf.fr)
rm(el.ff.cf, el.ff.fr)
rm(el.mm.cf, el.mm.fr)

# Building vertex attributes (females)
b2 <- subset(b1, sex1 == 'f', select = c("agent", "n", "mySID", "x1", "y1"))
b2$agent <- factor(b2$agent)
tmpNames <- levels(b2$agent)
b2 <- subset(b2, n == 1)
ffNames <- vector(length = nFFAg, mode = "character")
ffXY <- matrix(nrow =  nFFAg, ncol = 2)
n <- length(tmpNames)
for (i in 1:n) {
    idx <- grep(tmpNames[i], b2$agent)
    if (length(idx) > 1) print(c(i, idx))
    id <- b2$mySID[idx]
    ffXY[id, 1] <- b2$x1[idx]
    ffXY[id, 2] <- b2$y1[idx]
    ffNames[id] <- as.character(b2$agent[idx])
}
rm(b1, b2, tmpNames, n, i, id, idx)

if (!TOOBIG) {
    ff.df <- data.frame(ffNames, ff.fr.bt, ff.fr.deg, ff.fr.prest, ff.frnt.bt,
                        ff.frnt.deg, ff.frnt.prest, ff.frntrf.bt, ff.frntrf.deg, ff.frntrf.prest,
                        ff.en.bt, ff.en.deg, ff.en.prest)

    rm(ff.frnt.bt, ff.frnt.deg,
       ff.frnt.prest, ff.frntrf.bt, ff.frntrf.deg, ff.frntrf.prest, ff.en.bt,
       ff.en.deg, ff.en.prest)
}

rm(ffNames, ff.fr.bt, ff.fr.deg, ff.fr.prest)

cat("Location: Deleted ff.vectors\n")
ls()

if (TOOBIG) {
    sink()
    dev.off()
    warnings()
    quit(save = "no")
}

names(ff.df) <- c("name", "fr.bt", "fr.deg", "fr.prest", "frnt.bt", "frnt.deg",
                  "frnt.prest", "frntrf.bt", "frntrf.deg", "frntrf.prest", "en.bt", "en.deg",
                  "en.prest")
names(mm.df) <- c("name", "fr.bt", "fr.deg", "fr.prest", "frnt.bt", "frnt.deg",
                  "frnt.prest", "frntrf.bt", "frntrf.deg", "frntrf.prest", "en.bt", "en.deg",
                  "en.prest")

tmp <- rbind(mm.df, ff.df)

lp2 <- merge(lp, tmp)
lp2$sex <- factor(lp2$sex, levels = c(0, 1), labels = c("male", "female"))

rm(ff.df, mm.df, lp, tmp)

ff.df <- subset(lp2, sex == "female")
mm.df <- subset(lp2, sex == "male")
rm(lp2)


# Exploratory stepwise regressions

sink()

lm.tmp <- step(lm(fr.bt ~ age + loyalty + gratitude + gratStrategy +
                  fairness + vengefulness + vengStrategy + vengStr2 + meatValue + askMeatOnly +
                  bnvlcTOtherSex + bnvlcTSameSex + bnvlcTMother + bnvlcTSibling + bnvlcTChild +
                  maleEnergyImportance + maleAgeImportance + sameSexSel + energySel + childSel +
                  motherSel + siblingSel + friendSel + otherSexSel + oestrFemSel +
                  migAgeImportance + migFriendImportance + energy + generosity + meatGenerosity +
                  pity + envy + mutationRate + childhood + maxAge + bestMaleAge +
                  femalePromiscuity + kidValueForMale + nLiveChld + migPref + bravery + audacity +
                  migrating + acceptMigInv + acceptMoveInvtFromF + acceptMoveInvtFromM + huntValue +
                  patrolValue + begStrategy + huntStrategy + huntInterval + maxHuntPatrolSize +
                  vOfFStranger + vOfMStranger + enmityThr + lowDeficit + hiDeficit + tmFct +
                  xenophTM + xenophTF + xenophTFwK + fearOfHPWhenHasKid + fearOfHPatches +
                  fvalueOfNoSh + fvalueOfNoH + fvalueOfNoCT + mvalueOfNoSh + mvalueOfNoH +
                  mvalueOfNoCT + adviceValue + valueOfNoSex + fvalueOfNotFair + mvalueOfNotFair +
                  norm + metanorm + terriRemStrategy + hasShame + zeroPostvNo + expFFairness +
                  expMFairness, mm.df), trace = -1)

sink("statistics", append = TRUE)
cat(" ===================================================================== \n")
cat("Betweenness of males (friends network):\n\n")
summary(lm.tmp)
rm(lm.tmp)
sink()

lm.tmp <- step(lm(fr.bt ~ age + loyalty + gratitude + gratStrategy +
                  fairness + vengefulness + vengStrategy + vengStr2 + meatValue + askMeatOnly +
                  bnvlcTOtherSex + bnvlcTSameSex + bnvlcTMother + bnvlcTSibling + bnvlcTChild +
                  maleEnergyImportance + maleAgeImportance + sameSexSel + energySel + childSel +
                  motherSel + siblingSel + friendSel + otherSexSel + oestrFemSel +
                  migAgeImportance + migFriendImportance + energy + generosity + meatGenerosity +
                  pity + envy + mutationRate + childhood + maxAge + bestMaleAge +
                  femalePromiscuity + kidValueForMale + nLiveChld + migPref + bravery + audacity +
                  migrating + acceptMigInv + acceptMoveInvtFromF + acceptMoveInvtFromM + huntValue
              + patrolValue + begStrategy + huntStrategy + huntInterval + maxHuntPatrolSize +
                  vOfFStranger + vOfMStranger + enmityThr + lowDeficit + hiDeficit + tmFct +
                  xenophTM + xenophTF + xenophTFwK + fearOfHPWhenHasKid + fearOfHPatches +
                  fvalueOfNoSh + fvalueOfNoH + fvalueOfNoCT + mvalueOfNoSh + mvalueOfNoH +
                  mvalueOfNoCT + adviceValue + valueOfNoSex + fvalueOfNotFair + mvalueOfNotFair +
                  norm + metanorm + terriRemStrategy + hasShame + zeroPostvNo + expFFairness +
                  expMFairness, ff.df), trace = -1)

sink("statistics", append = TRUE)
cat(" ===================================================================== \n")
cat("Betweenness of females (friends network):\n\n")
summary(lm.tmp)
rm(lm.tmp)
sink()

lm.tmp <- step(lm(fr.deg ~ age + loyalty + gratitude + gratStrategy +
                  fairness + vengefulness + vengStrategy + vengStr2 + meatValue + askMeatOnly +
                  bnvlcTOtherSex + bnvlcTSameSex + bnvlcTMother + bnvlcTSibling + bnvlcTChild +
                  maleEnergyImportance + maleAgeImportance + sameSexSel + energySel + childSel +
                  motherSel + siblingSel + friendSel + otherSexSel + oestrFemSel +
                  migAgeImportance + migFriendImportance + energy + generosity + meatGenerosity +
                  pity + envy + mutationRate + childhood + maxAge + bestMaleAge +
                  femalePromiscuity + kidValueForMale + nLiveChld + migPref + bravery + audacity +
                  migrating + acceptMigInv + acceptMoveInvtFromF + acceptMoveInvtFromM + huntValue +
                  patrolValue + begStrategy + huntStrategy + huntInterval + maxHuntPatrolSize +
                  vOfFStranger + vOfMStranger + enmityThr + lowDeficit + hiDeficit + tmFct +
                  xenophTM + xenophTF + xenophTFwK + fearOfHPWhenHasKid + fearOfHPatches +
                  fvalueOfNoSh + fvalueOfNoH + fvalueOfNoCT + mvalueOfNoSh + mvalueOfNoH +
                  mvalueOfNoCT + adviceValue + valueOfNoSex + fvalueOfNotFair + mvalueOfNotFair +
                  norm + metanorm + terriRemStrategy + hasShame + zeroPostvNo + expFFairness +
                  expMFairness, mm.df), trace = -1)

sink("statistics", append = TRUE)
cat(" ===================================================================== \n")
cat("Degree of centrality of males (friends network):\n\n")
summary(lm.tmp)
rm(lm.tmp)
sink()

lm.tmp <- step(lm(fr.deg ~ age + loyalty + gratitude + gratStrategy +
                  fairness + vengefulness + vengStrategy + vengStr2 + meatValue + askMeatOnly +
                  bnvlcTOtherSex + bnvlcTSameSex + bnvlcTMother + bnvlcTSibling + bnvlcTChild +
                  maleEnergyImportance + maleAgeImportance + sameSexSel + energySel + childSel +
                  motherSel + siblingSel + friendSel + otherSexSel + oestrFemSel +
                  migAgeImportance + migFriendImportance + energy + generosity + meatGenerosity +
                  pity + envy + mutationRate + childhood + maxAge + bestMaleAge +
                  femalePromiscuity + kidValueForMale + nLiveChld + migPref + bravery + audacity +
                  migrating + acceptMigInv + acceptMoveInvtFromF + acceptMoveInvtFromM + huntValue +
                  patrolValue + begStrategy + huntStrategy + huntInterval + maxHuntPatrolSize +
                  vOfFStranger + vOfMStranger + enmityThr + lowDeficit + hiDeficit + tmFct +
                  xenophTM + xenophTF + xenophTFwK + fearOfHPWhenHasKid + fearOfHPatches +
                  fvalueOfNoSh + fvalueOfNoH + fvalueOfNoCT + mvalueOfNoSh + mvalueOfNoH +
                  mvalueOfNoCT + adviceValue + valueOfNoSex + fvalueOfNotFair + mvalueOfNotFair +
                  norm + metanorm + terriRemStrategy + hasShame + zeroPostvNo + expFFairness +
                  expMFairness, ff.df), trace = -1)

sink("statistics", append = TRUE)
cat(" ===================================================================== \n")
cat("Degree of centrality of females (friends network):\n\n")
summary(lm.tmp)
rm(lm.tmp)
sink()

lm.tmp <- step(lm(fr.prest ~ age + loyalty + gratitude + gratStrategy +
                  fairness + vengefulness + vengStrategy + vengStr2 + meatValue + askMeatOnly +
                  bnvlcTOtherSex + bnvlcTSameSex + bnvlcTMother + bnvlcTSibling + bnvlcTChild +
                  maleEnergyImportance + maleAgeImportance + sameSexSel + energySel + childSel +
                  motherSel + siblingSel + friendSel + otherSexSel + oestrFemSel +
                  migAgeImportance + migFriendImportance + energy + generosity + meatGenerosity +
                  pity + envy + mutationRate + childhood + maxAge + bestMaleAge +
                  femalePromiscuity + kidValueForMale + nLiveChld + migPref + bravery + audacity +
                  migrating + acceptMigInv + acceptMoveInvtFromF + acceptMoveInvtFromM + huntValue +
                  patrolValue + begStrategy + huntStrategy + huntInterval + maxHuntPatrolSize +
                  vOfFStranger + vOfMStranger + enmityThr + lowDeficit + hiDeficit + tmFct +
                  xenophTM + xenophTF + xenophTFwK + fearOfHPWhenHasKid + fearOfHPatches +
                  fvalueOfNoSh + fvalueOfNoH + fvalueOfNoCT + mvalueOfNoSh + mvalueOfNoH +
                  mvalueOfNoCT + adviceValue + valueOfNoSex + fvalueOfNotFair + mvalueOfNotFair +
                  norm + metanorm + terriRemStrategy + hasShame + zeroPostvNo + expFFairness +
                  expMFairness, mm.df), trace = -1)

sink("statistics", append = TRUE)
cat(" ===================================================================== \n")
cat("Prestige of males (friends network):\n\n")
summary(lm.tmp)
rm(lm.tmp)
sink()

lm.tmp <- step(lm(fr.prest ~ age + loyalty + gratitude + gratStrategy +
                  fairness + vengefulness + vengStrategy + vengStr2 + meatValue + askMeatOnly +
                  bnvlcTOtherSex + bnvlcTSameSex + bnvlcTMother + bnvlcTSibling + bnvlcTChild +
                  maleEnergyImportance + maleAgeImportance + sameSexSel + energySel + childSel +
                  motherSel + siblingSel + friendSel + otherSexSel + oestrFemSel +
                  migAgeImportance + migFriendImportance + energy + generosity + meatGenerosity +
                  pity + envy + mutationRate + childhood + maxAge + bestMaleAge +
                  femalePromiscuity + kidValueForMale + nLiveChld + migPref + bravery + audacity +
                  migrating + acceptMigInv + acceptMoveInvtFromF + acceptMoveInvtFromM + huntValue +
                  patrolValue + begStrategy + huntStrategy + huntInterval + maxHuntPatrolSize +
                  vOfFStranger + vOfMStranger + enmityThr + lowDeficit + hiDeficit + tmFct +
                  xenophTM + xenophTF + xenophTFwK + fearOfHPWhenHasKid + fearOfHPatches +
                  fvalueOfNoSh + fvalueOfNoH + fvalueOfNoCT + mvalueOfNoSh + mvalueOfNoH +
                  mvalueOfNoCT + adviceValue + valueOfNoSex + fvalueOfNotFair + mvalueOfNotFair +
                  norm + metanorm + terriRemStrategy + hasShame + zeroPostvNo + expFFairness +
                  expMFairness, ff.df), trace = -1)

sink("statistics", append = TRUE)
cat(" ===================================================================== \n")
cat("Prestige of females (friends network):\n\n")
summary(lm.tmp)
rm(lm.tmp)
sink()

lm.tmp <- step(lm(fr.deg ~ age + loyalty + gratitude + gratStrategy +
                  fairness + vengefulness + vengStrategy + vengStr2 + meatValue + askMeatOnly +
                  bnvlcTOtherSex + bnvlcTSameSex + bnvlcTMother + bnvlcTSibling + bnvlcTChild +
                  maleEnergyImportance + maleAgeImportance + sameSexSel + energySel + childSel +
                  motherSel + siblingSel + friendSel + otherSexSel + oestrFemSel +
                  migAgeImportance + migFriendImportance + energy + generosity + meatGenerosity +
                  pity + envy + mutationRate + childhood + maxAge + bestMaleAge +
                  femalePromiscuity + kidValueForMale + nLiveChld + migPref + bravery + audacity +
                  migrating + acceptMigInv + acceptMoveInvtFromF + acceptMoveInvtFromM + huntValue +
                  patrolValue + begStrategy + huntStrategy + huntInterval + maxHuntPatrolSize +
                  vOfFStranger + vOfMStranger + enmityThr + lowDeficit + hiDeficit + tmFct +
                  xenophTM + xenophTF + xenophTFwK + fearOfHPWhenHasKid + fearOfHPatches +
                  fvalueOfNoSh + fvalueOfNoH + fvalueOfNoCT + mvalueOfNoSh + mvalueOfNoH +
                  mvalueOfNoCT + adviceValue + valueOfNoSex + fvalueOfNotFair + mvalueOfNotFair +
                  norm + metanorm + terriRemStrategy + hasShame + zeroPostvNo + expFFairness +
                  expMFairness, mm.df), trace = -1)

sink("statistics", append = TRUE)
cat(" ===================================================================== \n")
cat("Degree of centrality of males (friends network):\n\n")
summary(lm.tmp)
rm(lm.tmp)
sink()

lm.tmp <- step(lm(fr.deg ~ age + loyalty + gratitude + gratStrategy +
                  fairness + vengefulness + vengStrategy + vengStr2 + meatValue + askMeatOnly +
                  bnvlcTOtherSex + bnvlcTSameSex + bnvlcTMother + bnvlcTSibling + bnvlcTChild +
                  maleEnergyImportance + maleAgeImportance + sameSexSel + energySel + childSel +
                  motherSel + siblingSel + friendSel + otherSexSel + oestrFemSel +
                  migAgeImportance + migFriendImportance + energy + generosity + meatGenerosity +
                  pity + envy + mutationRate + childhood + maxAge + bestMaleAge +
                  femalePromiscuity + kidValueForMale + nLiveChld + migPref + bravery + audacity +
                  migrating + acceptMigInv + acceptMoveInvtFromF + acceptMoveInvtFromM + huntValue +
                  patrolValue + begStrategy + huntStrategy + huntInterval + maxHuntPatrolSize +
                  vOfFStranger + vOfMStranger + enmityThr + lowDeficit + hiDeficit + tmFct +
                  xenophTM + xenophTF + xenophTFwK + fearOfHPWhenHasKid + fearOfHPatches +
                  fvalueOfNoSh + fvalueOfNoH + fvalueOfNoCT + mvalueOfNoSh + mvalueOfNoH +
                  mvalueOfNoCT + adviceValue + valueOfNoSex + fvalueOfNotFair + mvalueOfNotFair +
                  norm + metanorm + terriRemStrategy + hasShame + zeroPostvNo + expFFairness +
                  expMFairness, ff.df), trace = -1)

sink("statistics", append = TRUE)
cat(" ===================================================================== \n")
cat("Degree of centrality of females (friends network):\n\n")
summary(lm.tmp)
rm(lm.tmp)

rm(mm.df, ff.df)

sink()
dev.off()

ls()

warnings()
quit(save = "no")

sink()
dev.off()

ls()
warnings()
quit(save = "no")


# Old code using matrix and the sna package

#nVert <- length(agNames)
#nIso <- length(isolates(g.friend))
#mainTitle <- paste("Friends (n = ", nVert, ", isolates = ", nIso, ")", sep = "")
#gplot(g.friend, vertex.col = agSex.color, edge.col = g.kinship, main = mainTitle, object.scale = 0.006)
#legend("topleft", legend = c("Stranger", "Sibling", "Mother", "Child"), col = c("black", "green3", "red", "blue"),
#lty = 1, bty = "n")


# Network of male agents who have common friends
for (i in 1:mlen) {
    gm[as.character(mm$agent[i]), as.character(mm$other[i])] <- mm$nSF[i]
}
strongTie <- quantile(mm$nSF)[[4]]
weakTie <- quantile(mm$nSF)[[3]]

mainTitle <- paste("Network of males who share friends (strong ties, n > ", strongTie, ")", sep = "")
gplot(gm, main = mainTitle, thresh = strongTie, vertex.col = 4, edge.col = gm.kinship.color, object.scale = 0.006)
if (weakTie < strongTie) {
    mainTitle <- paste("Network of males who share friends (weak ties, n > ", weakTie, ")", sep = "")
    gplot(gm, main = mainTitle, thresh = weakTie, vertex.col = 4, edge.col = gm.kinship.color, object.scale = 0.006)
}

plot(NULL, main = "Map of weak and strong ties between males\naccording to number of common friends",
     xlab = "x", ylab = "y", xlim = c(0, maxX), ylim = c(0, maxY))
if (weakTie < strongTie) {
    bties <- subset(m, nSF <= weakTie & (!((x1 == x2) & (y1 == y2))))
    if (length(bties$x1) > 0) arrows(bties$x1, bties$y1, bties$x2, bties$y2, col = 5, length = 0, lwd = 1)
    bties <- subset(m, nSF > weakTie & nSF <= strongTie & (!((x1 == x2) & (y1 == y2))))
    if (length(bties$x1) > 0) arrows(bties$x1, bties$y1, bties$x2, bties$y2, col = 4, length = 0, lwd = 1)
}
bties <- subset(m, nSF > strongTie & (!((x1 == x2) & (y1 == y2))))
if (length(bties$x1) > 0) arrows(bties$x1, bties$y1, bties$x2, bties$y2, col = "blue", length = 0, lwd = 2)
if (weakTie < strongTie) {
    bties <- subset(m, nSF <= weakTie & x1 == x2 & y1 == y2)
    if (length(bties$x1) > 0) points(bties$x1, bties$y1, col = "black", lwd = 0.5, cex = 0.3)
    bties <- subset(m, nSF > weakTie & nSF <= strongTie & x1 == x2 & y1 == y2)
    if (length(bties$x1) > 0) points(bties$x1, bties$y1, col = "black", cex = 0.6)
}
bties <- subset(m, nSF > strongTie & x1 == x2 & y1 == y2)
if (length(bties$x1) > 0) points(bties$x1, bties$y1, col = "black", lwd = 1.5, cex = 1.2)

# Network of agents who have common friends (females)
for (i in 1:flen) {
    gf[as.character(ff$agent[i]), as.character(ff$other[i])] <- ff$nSF[i]
}
strongTie <- quantile(ff$nSF)[[4]]
weakTie <- quantile(ff$nSF)[[3]]

mainTitle <- paste("Network of females who share friends (strong ties, n > ", strongTie, ")", sep = "")
gplot(gf, main = mainTitle, thresh = strongTie, edge.col = gf.kinship, object.scale = 0.006)
if (weakTie < strongTie) {
    mainTitle <- paste("Network of females who share friends (weak ties, n > ", weakTie, ")", sep = "")
    gplot(gf, main = mainTitle, thresh = weakTie, edge.col = gf.kinship, object.scale = 0.006)
}

plot(NULL, main = "Map of weak and strong ties between females\naccording to number of common friends",
     xlab = "x", ylab = "y", xlim = c(0, maxX), ylim = c(0, maxY))
if (weakTie < strongTie) {
    bties <- subset(f, nSF <= weakTie & (!((x1 == x2) & (y1 == y2))))
    if (length(bties$x1) > 0) arrows(bties$x1, bties$y1, bties$x2, bties$y2, col = 5, length = 0, lwd = 1)
    bties <- subset(f, nSF > weakTie & nSF <= strongTie & (!((x1 == x2) & (y1 == y2))))
    if (length(bties$x1) > 0) arrows(bties$x1, bties$y1, bties$x2, bties$y2, col = 4, length = 0, lwd = 1)
}
bties <- subset(f, nSF > strongTie & (!((x1 == x2) & (y1 == y2))))
if (length(bties$x1) > 0) arrows(bties$x1, bties$y1, bties$x2, bties$y2, col = "blue", length = 0, lwd = 2)
if (weakTie < strongTie) {
    bties <- subset(f, nSF <= weakTie & x1 == x2 & y1 == y2)
    if (length(bties$x1) > 0) points(bties$x1, bties$y1, col = "black", lwd = 0.5, cex = 0.3)
    bties <- subset(f, nSF > weakTie & nSF <= strongTie & x1 == x2 & y1 == y2)
    if (length(bties$x1) > 0) points(bties$x1, bties$y1, col = "black", cex = 0.6)
}
bties <- subset(f, nSF > strongTie & x1 == x2 & y1 == y2)
if (length(bties$x1) > 0) points(bties$x1, bties$y1, col = "black", lwd = 1.5, cex = 1.2)

# Network of agents who have common negative memories (males)
for (i in 1:mlen) {
    gm[as.character(mm$agent[i]), as.character(mm$other[i])] <- mm$nSE[i]
}
strongTie <- quantile(mm$nSE)[[4]]
weakTie <- quantile(mm$nSE)[[3]]

mainTitle <- paste("Network of males who share negative memories (strong ties, n > ", strongTie, ")", sep = "")
gplot(gm, main = mainTitle, thresh = strongTie, vertex.col = 4, edge.col = gm.kinship.color, object.scale = 0.006)
if (weakTie < strongTie) {
    mainTitle <- paste("Network of males who share negative memories (weak ties, n > ", weakTie, ")", sep = "")
    gplot(gm, main = mainTitle, thresh = weakTie, vertex.col = 4, edge.col = gm.kinship.color, object.scale = 0.006)
}

plot(NULL, main = "Map of weak and strong ties between males\naccording to number of common negative memories",
     xlab = "x", ylab = "y", xlim = c(0, maxX), ylim = c(0, maxY))
if (weakTie < strongTie) {
    bties <- subset(m, nSE <= weakTie & (!((x1 == x2) & (y1 == y2))))
    if (length(bties$x1) > 0) arrows(bties$x1, bties$y1, bties$x2, bties$y2, col = 10, length = 0, lwd = 1)
    bties <- subset(m, nSE > weakTie & nSE <= strongTie & (!((x1 == x2) & (y1 == y2))))
    if (length(bties$x1) > 0) arrows(bties$x1, bties$y1, bties$x2, bties$y2, col = 2, length = 0, lwd = 1)
}
bties <- subset(m, nSE > strongTie & (!((x1 == x2) & (y1 == y2))))
if (length(bties$x1) > 0) arrows(bties$x1, bties$y1, bties$x2, bties$y2, col = "red", length = 0, lwd = 2)
if (weakTie < strongTie) {
    bties <- subset(m, nSE <= weakTie & x1 == x2 & y1 == y2)
    if (length(bties$x1) > 0) points(bties$x1, bties$y1, col = "black", lwd = 0.5, cex = 0.3)
    bties <- subset(m, nSE > weakTie & nSE <= strongTie & x1 == x2 & y1 == y2)
    if (length(bties$x1) > 0) points(bties$x1, bties$y1, col = "black", cex = 0.6)
}
bties <- subset(m, nSE > strongTie & x1 == x2 & y1 == y2)
if (length(bties$x1) > 0) points(bties$x1, bties$y1, col = "black", lwd = 1.5, cex = 1.2)

# Network of agents who have common negative memories (females)
for (i in 1:flen) {
    gf[as.character(ff$agent[i]), as.character(ff$other[i])] <- ff$nSE[i]
}
strongTie <- quantile(ff$nSE)[[4]]
weakTie <- quantile(ff$nSE)[[3]]

mainTitle <- paste("Network of females who share negative memories (strong ties, n > ", strongTie, ")", sep = "")
gplot(gf, main = mainTitle, thresh = strongTie, edge.col = gf.kinship, object.scale = 0.006)
if (weakTie < strongTie) {
    mainTitle <- paste("Network of females who share negative memories (weak ties, n > ", weakTie, ")", sep = "")
    gplot(gf, main = mainTitle, thresh = weakTie, edge.col = gf.kinship, object.scale = 0.006)
}
rm(mainTitle, ff, mm, g, gf, gm, blen, flen, mlen, i)

plot(NULL, main = "Map of weak and strong ties between females\naccording to number of common negative memories",
     xlab = "x", ylab = "y", xlim = c(0, maxX), ylim = c(0, maxY))
if (weakTie < strongTie) {
    bties <- subset(f, nSE <= weakTie & (!((x1 == x2) & (y1 == y2))))
    if (length(bties$x1) > 0) arrows(bties$x1, bties$y1, bties$x2, bties$y2, col = 10, length = 0, lwd = 1)
    bties <- subset(f, nSE > weakTie & nSE <= strongTie & (!((x1 == x2) & (y1 == y2))))
    if (length(bties$x1) > 0) arrows(bties$x1, bties$y1, bties$x2, bties$y2, col = 2, length = 0, lwd = 1)
}
bties <- subset(f, nSE > strongTie & (!((x1 == x2) & (y1 == y2))))
if (length(bties$x1) > 0) arrows(bties$x1, bties$y1, bties$x2, bties$y2, col = "red", length = 0, lwd = 2)
if (weakTie < strongTie) {
    bties <- subset(f, nSE <= weakTie & x1 == x2 & y1 == y2)
    if (length(bties$x1) > 0) points(bties$x1, bties$y1, col = "black", lwd = 0.5, cex = 0.3)
    bties <- subset(f, nSE > weakTie & nSE < strongTie & x1 == x2 & y1 == y2)
    if (length(bties$x1) > 0) points(bties$x1, bties$y1, col = "black", cex = 0.6)
}
bties <- subset(f, nSE > strongTie & x1 == x2 & y1 == y2)
if (length(bties$x1) > 0) points(bties$x1, bties$y1, col = "black", lwd = 1.5, cex = 1.2)

rm(bties, strongTie, weakTie, maxX, maxY)

# vim:tw=300
