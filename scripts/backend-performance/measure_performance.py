#!/usr/bin/env python3

HELP_TEXT = """
The purpose of this script is to measure cbmc performance across a selection of
back ends / solvers. The intention is for maintainers to be able to run it
locally, not for integration with CI or cloud services. This is in order keep it
relatively simple to read / maintain and use, as well as to aid portability
though by avoiding platform dependencies.
"""

# ********* Managing dependencies for running this script **********************
# pipenv based dependency management is provided alongside this script, using
# the `pipfile` to manage the dependencies needed and `pipfile.lock` to specify
# the versions of these dependencies.
#
# Prerequisites to be resolved using your OS platform's installation method of
# choice -
#  * python3
#  * pip
#
# To resolve dependencies -
#  * Install pipenv, if you don't have it already.
#    `python3 -m pip install pipenv`
