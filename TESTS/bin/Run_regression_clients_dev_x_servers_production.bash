#!/bin/bash

## October 2019, Carlos Jose Mazieri



exe_path=`dirname $0`

${exe_path}/Run_regression_generic.bash --prod-servers-only --dev-clients-only $*
${exe_path}/Test_external_intercom.bash --prod-servers-only --dev-clients-only $*
