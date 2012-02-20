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

##### STAT HASH #####
our %stat_ext = ('ext_html'=>{disp=>'html', count=>0, pattern=>'.htm$|.html$|.shtml$'},
        'ext_php'=>{disp=>'php',  count=>0, pattern=>'.php$'},
        'ext_cgi'=>{disp=>'cgi',  count=>0, pattern=>'.cgi$'},
        'ext_jpg'=>{disp=>'jpg',  count=>0, pattern=>'.jpg$|.jpeg$'},
        'ext_png'=>{disp=>'png',  count=>0, pattern=>'.png$'},
        'ext_gif'=>{disp=>'gif',  count=>0, pattern=>'.gif$'},
        'ext_pdf'=>{disp=>'pdf',  count=>0, pattern=>'.pdf$'},
        'ext_ico'=>{disp=>'ico',  count=>0, pattern=>'.ico$'},
        'ext_lzh'=>{disp=>'lzh',  count=>0, pattern=>'.lzh$'},
        'ext_zip'=>{disp=>'zip',  count=>0, pattern=>'.zip$|.gz$'},
        'ext_css'=>{disp=>'css',  count=>0, pattern=>'.css$'},
        'ext_js'=>{disp=>'js',  count=>0, pattern=>'.js$'},
        'ext_xml'=>{disp=>'xml',  count=>0, pattern=>'.xml$'},
        'ext_txt'=>{disp=>'txt',  count=>0, pattern=>'.txt$'},
        'ext_root'=>{disp=>'/',  count=>0, pattern=>'/$'},
        'ext__other'=>{disp=>'other',  count=>0, pattern=>''},
        'ext__error'=>{disp=>'error',  count=>0, pattern=>''},
        'ext__all'=>{disp=>'all',  count=>0, pattern=>''}
        );

our %stat_statuscode = ('code_2xx'=>{disp=>'2xx', count=>0, min=>200, max=>299},
        'code_3xx'=>{disp=>'3xx',  count=>0, min=>300, max=>399},
        'code_400'=>{disp=>'400-402',  count=>0, min=>400, max=>402},
        'code_403'=>{disp=>'403',  count=>0, min=>403, max=>403},
        'code_404'=>{disp=>'404',  count=>0, min=>404, max=>404},
        'code_4xx'=>{disp=>'405-499',  count=>0, min=>405, max=>499},
        'code_5xx'=>{disp=>'5xx',  count=>0, min=>500, max=>599}
        );

##### STAT HASH #####

1
