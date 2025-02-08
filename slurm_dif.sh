#!/bin/bash                                                                                                                                                                                                                            

#SBATCH -J QPIX_Marley      # A single job name for the array                                                                                                                                                                          
#SBATCH -n 1                # Number of cores                                                                                                                                                                                          
#SBATCH -N 1                # All cores on one machine                                                                                                                                                                                 
#SBATCH -p guenette         # Partition                                                                                                                                                                                                
#SBATCH --mem 1000          # Memory request (Mb)                                                                                                                                                                                      
#SBATCH -t 0-8:00           # Maximum execution time (D-HH:MM)                                                                                                                                                                         
#SBATCH -o %A_%a.out        # Standard output                                                                                                                                                                                          
#SBATCH -e %A_%a.err        # Standard error                                                                                                                                                                                           


## Job options                                                                                                                                                                                                                         
JOBNUMBER=${SLURM_ARRAY_TASK_ID}

SEED=`echo "scale = 2;  $JOBNUMBER" | bc`
NAME="Am241-${SEED}"
OUT="${NAME}.root"

MACRO="Am241-${NAME}.mac"
OUTFILE="/media/argon/DATA/Ilker/Diffusion/output/${OUT}"

#OUTPUTDIR='/n/holystore01/LABS/guenette_lab/Lab/data/QPIX/Supernova_Test/nu_e/'

#MARLEY_json='/n/holystore01/LABS/guenette_lab/Users/amcdonald/Supernova_Qpix/Q_PIX_GEANT4/cfg/marley_config_supernova.js'
#HIST_file='/n/holystore01/LABS/guenette_lab/Users/amcdonald/Supernova_Qpix/nusperbin2d.root'

## making the macro                                                                                                                                                                                                                    
INPUT_MACRO="/n/holyscratch01/guenette_lab/${MACRO}"
echo $INPUT_MACRO

echo "/Inputs/root_output ${OUTFILE}"                 >>${INPUT_MACRO}
echo "/QPIX/RegisterMacro macros/slurm_Am241.mac"  >>${INPUT_MACRO}





/n/holystore01/LABS/guenette_lab/Users/amcdonald/Supernova_Qpix/Q_PIX_GEANT4/Build/app/G4Basic ${INPUT_MACRO} 

#mv "${OUTFILE}" "${OUTPUTDIR}/"
