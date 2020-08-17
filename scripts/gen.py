import argparse
import xml.etree.ElementTree as ET
import os
from jinja2 import Environment, FileSystemLoader
import math
import sys

context = {}
print('number of arguments : ' + str(len(sys.argv)))

#### system variables
context['NUM_CORES'] = int(sys.argv[1])
context['LLFULLDB'] = sys.argv[2]
context['CRUMPLEINONDELETE'] = sys.argv[3]
context['WORK_BLOCK_HEIGHT'] = int(sys.argv[4])

print ('GraphTinker configured parameters (Courtesy: Jinja 2.0)...')
print ('NUM_CORES: ' + str(context['NUM_CORES']))
print ('LLFULLDB: ' + str(context['LLFULLDB']))
print ('CRUMPLEINONDELETE: ' + str(context['CRUMPLEINONDELETE']))
print ('WORK_BLOCK_HEIGHT: ' + str(context['WORK_BLOCK_HEIGHT']))
	
#================================
o_path0="../test/graphtinker/updates_test.cpp"
o_path1="../include/graphtinker/common.h"

out_path0=os.path.abspath(o_path0)
out_path1=os.path.abspath(o_path1)

templ_path0="../test/graphtinker"
templ_path1="../include/graphtinker"

#####################
### System parameters (gds main)

#####################
### System parameters (gds LL)
context['LVACOARSENESSWIDTH'] = 2048
context['LLEDGEBLOCKSIZE'] = 512
		
#####################
### Sequencers
context['NUM_CORES_seq'] = []
for i in range (1,(context['NUM_CORES']+1)):
		context['NUM_CORES_seq'].append(i)

#=========================
#=== all templates ===
env0 = Environment(loader=FileSystemLoader(os.path.abspath(templ_path0)), trim_blocks=True, lstrip_blocks=True)
env1 = Environment(loader=FileSystemLoader(os.path.abspath(templ_path1)), trim_blocks=True, lstrip_blocks=True)

env0.globals.update(zip=zip)
env1.globals.update(zip=zip)

template0 = env0.get_template('updates_test.template')
template1 = env1.get_template('common_h.template')

# Render the template for the output file.
rendered_file0 = template0.render(context=context)
rendered_file1 = template1.render(context=context)

# Write output file
with open(out_path0, 'w') as outFile0:
	outFile0.write(rendered_file0)

with open(out_path1, 'w') as outFile1:
	outFile1.write(rendered_file1)

print ("successful!")
print ("...")


