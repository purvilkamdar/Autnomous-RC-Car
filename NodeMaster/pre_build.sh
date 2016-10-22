#!/bin/bash

# 
# This file executes from the build directory by the Eclipse "Pre Build" step
#

python ../_can_dbc/dbc_parse.py -i C:/SJSU/CMPE243/rc/TITANS_PROJECT_RC_CAR/243.dbc -s MASTER > ../_can_dbc/generated_can.h