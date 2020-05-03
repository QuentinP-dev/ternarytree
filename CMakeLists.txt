# Copyright (C) 2020 Quentin Putaud
# Distributed under the GNU General Public License version 3
# (http://www.gnu.org/licenses)

cmake_minimum_required(VERSION 3.8)

# ------- config -------

set(PROJECT_NAME TernaryTree)
set(PROJECT_VERSION 1.0)
set(PROJECT_BRIEF "A &nbsp; template &nbsp; class &nbsp; of &nbsp; ternary &nbsp; tree")

project(${PROJECT_NAME} VERSION ${PROJECT_VERSION})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ----------------------

find_package(Doxygen)
add_subdirectory(docs)

