#! bin/bash

make -C Server all
make -C Client all
make -C UI all

make -C UI run
