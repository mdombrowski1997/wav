#!/usr/bin/Rscript

#open stdin connection
f <- file("stdin")
open(f)
#init sequence
s <- 0
#ignore inevitable EOF error on first line
line<-readLines(f,n=1)

#read from stdin
while(length(line <- readLines(f,n=1)) > 0)
{
    s <- c(s, line)
}

#cast to number
s <- as.numeric(s)

#perform DTFT and print
S <- dtft(s)
print.simple.list(S)
