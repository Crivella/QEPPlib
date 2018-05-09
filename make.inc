#Library env variables

PREFIX=$(HOME)/.local

INC =./inc
SDIR=./src
ODIR=./obj
CURDIR=$(shell pwd)
LDIR=$(PREFIX)/lib
IDIR=$(PREFIX)/include

LD_FLAGS=-D__LIBXML2

CC=gcc
MPICC=gcc
CFLAGS=-g -O2 -Wall -fpic -std=c11 -I$(INC) $(LD_FLAGS)
