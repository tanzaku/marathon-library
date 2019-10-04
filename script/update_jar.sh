#!/bin/bash

set -eu

javac --release 8 JumpAroundVis.java
zip tester.jar *.class
rm *.class
