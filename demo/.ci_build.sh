#!/bin/bash
set -xe

sudo apt-get install -y libsdl2-dev

cd demo
make
