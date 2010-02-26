#!/bin/sh
cat << EoI | grep -v -e "^#"

	
Random_FAIL exam-ext-field-field04-run \
	slc4_amd64_gcc34_dbg	\
	i686-slc5-gcc43-opt
	
EXPECTED_FAIL exam-ext-g3tog4-clgeometry-build \
	osx105_ia32_gcc401	\
	osx105_ia32_gcc401_dbg
	i686-slc5-gcc43-dbg
			
EXPECTED_FAIL exam-ext-g3tog4-cltog4-build \
	osx105_ia32_gcc401	\
	osx105_ia32_gcc401_dbg

#  XercesC compile warnings ... fixed using XercesC 3.1 for slc5, 26 Feb 2010
#EXPECTED_FAIL exam-ext-persistency-gdml-g01-build 
#EXPECTED_FAIL exam-ext-persistency-gdml-g02-build 
#EXPECTED_FAIL exam-ext-persistency-gdml-g03-build 
#EXPECTED_FAIL bench-calo-FullCMS-build \

#9.2.ref05
#EXPECTED_FAIL test19-build \
#	x86_64-slc5-gcc43-opt	\
#	x86_64-slc5-gcc43-dbg	\
#	i686-slc5-gcc43-opt	\
#	i686-slc5-gcc43-dbg
		
#28-Apr
# 3-Nov back for slc4 64 bit opt, sent mail to Pete in October already, gone in 9.3 
#EXPECTED_FAIL test28-run \
#	slc4_amd64_gcc34
	
#	slc4_ia32_gcc34	

#28 Apr
#EXPECTED_FAIL test29-run \
#	slc4_amd64_gcc34_dbg	\
#	slc4_ia32_gcc34_dbg	\
#	i686-slc5-gcc43-dbg

#9.2.ref05
#EXPECTED_FAIL test31-run \
#	slc4_amd64_gcc34_dbg	\
#	slc4_ia32_gcc34_dbg	\
#	x86_64-slc5-gcc43-dbg	\
#	i686-slc5-gcc43-dbg
	
#9.2.ref? fixed in test code
#EXPECTED_FAIL test34-run \
#	slc4_amd64_gcc34	\
#	slc4_amd64_gcc34_dbg	\
#	slc4_ia32_gcc34		\
#	slc4_ia32_gcc34_dbg	\
#	x86_64-slc5-gcc43-opt	\
#	x86_64-slc5-gcc43-dbg	\
#	i686-slc5-gcc43-opt	\
#	i686-slc5-gcc43-dbg	\
#	osx105_ia32_gcc401	\
#	osx105_ia32_gcc401_dbg

#fixed April 09, but for slow cpu timeout 
#EXPECTED_FAIL test61-run \
#	slc4_ia32_gcc34_dbg

# corrected by tag hadr-incl-V09-02-01, Nov 09
#EXPECTED_FAIL test62-run \
#	slc4_amd64_gcc34_dbg	\
#	slc4_ia32_gcc34_dbg	\
#	x86_64-slc5-gcc43-dbg	\
#	i686-slc5-gcc43-dbg
	
#new test August 09 
EXPECTED_FAIL test12-FTFBIC-run \
       slc4_ia32_gcc34_dbg     \
       i686-slc5-gcc43-dbg     
#       i686-slc5-gcc43-opt     ok 26 Feb 2010  

#    ok 26 Feb 2010
#EXPECTED_FAIL test12-run \
#       i686-slc5-gcc43-dbg     

#new test Nov 09, i686-slc5 ok 26 Feb 2010
EXPECTED_FAIL test12-QGSBIC-run \
       slc4_ia32_gcc34_dbg     
#       i686-slc5-gcc43-dbg    

#new test Nov 09
EXPECTED_FAIL bench-calo-HadCalCMS-run1-LBE \
	slc4_amd64_gcc34	\
	slc4_amd64_gcc34_dbg	\
	slc4_ia32_gcc34		\
	slc4_ia32_gcc34_dbg	\
	x86_64-slc5-gcc43-opt	\
	x86_64-slc5-gcc43-dbg	\
	x86_64-slc5-gcc41-opt   \
	x86_64-slc5-gcc41-dbg   \
	i686-slc5-gcc43-opt	\
	i686-slc5-gcc43-dbg	\
	osx105_ia32_gcc401_dbg

EoI

exit

slc4_amd64_gcc34
slc4_amd64_gcc34_dbg

slc4_ia32_gcc34
slc4_ia32_gcc34_dbg

i686-slc5-gcc43
i686-slc5-gcc43-dbg

x86_64-slc5-gcc43
x86_64-slc5-gcc43-dbg

osx105_ia32_gcc401
osx105_ia32_gcc401_dbg

i686-winxp-vc9-dbg
