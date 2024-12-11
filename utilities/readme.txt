1. Description of "advanced_logging" folder
===========================================================================================================
   "advanced_logs_generator.py", "log_decoder.py" are the python files required for the Logging enhancement support.

2. Description of "postbuild_profile" folder
===========================================================================================================
The Post-Build file is provided to make it easy for the user to specify actions required on the project output binary
to create various types of image files. This tool converts the bin file into rps format.

3. Description of "Python" folder :
===========================================================================================================
   load_certificate.py : Script to load the Server/Client certificates and keys onto the module.
   load_certificate_with_inx.py : Script to load the Server/Client certificates along with index onto the module.

   USAGE :
   python load_certificate.py <certificate_type> <certificate_path>
   python load_certificate_with_inx.py <certificate_type> <certificate_index> <certificate_path>
