##### GLOBAL CONFIG VALUE #####
# LOG directory
our $log_directory = './var/log/';
# LOG file pattern ( string for sprintf('name%d%d%d', $y, $m, $d) )
our $log_filename_template = 'access_log_%04d%02d%02d';
# ERROR_LOG file pattern ( string for sprintf('name%d%d%d', $y, $m, $d) )
our $errorlog_filename_template = 'error_log_%04d%02d%02d';
# Output directory
our $output_directory = './var/www/';
# Output file pattern ( string for sprintf('name%d%d%d', $y, $m, $d) )
our $output_filename_template = 'log_report_%04d%02d%02d.html';
# Output files index filename
our $output_index_filename = 'log_index.html';
# Error Message Log Output (in Output directory)
our $error_message_filename = 'log_report_error.txt';
# use Compress::Zlib =1, not use (use gunzip command)=0
our $flag_use_zlib = 0;
##### GLOBAL CONFIG VALUE #####
1
