#!/usr/bin/perl

# save this file in << UTF-8  >> encode !
# ******************************************************
# Software name : parse-apache-log.pl
#
# Copyright (C) INOUE Hirokazu, All Rights Reserved
#     http://oasis.halfmoon.jp/
#
# parse-apache-log.pl
# version 0.1 (2012/Feb/04)
# version 0.2 (2012/Feb/07)
#
# GNU GPL Free Software
#
# このプログラムはフリーソフトウェアです。あなたはこれを、フリーソフトウェア財
# 団によって発行された GNU 一般公衆利用許諾契約書(バージョン2か、希望によっては
# それ以降のバージョンのうちどれか)の定める条件の下で再頒布または改変することが
# できます。
#
# このプログラムは有用であることを願って頒布されますが、*全くの無保証* です。
# 商業可能性の保証や特定の目的への適合性は、言外に示されたものも含め全く存在し
# ません。詳しくはGNU 一般公衆利用許諾契約書をご覧ください。
#
# あなたはこのプログラムと共に、GNU 一般公衆利用許諾契約書の複製物を一部受け取
# ったはずです。もし受け取っていなければ、フリーソフトウェア財団まで請求してく
# ださい(宛先は the Free Software Foundation, Inc., 59 Temple Place, Suite 330
# , Boston, MA 02111-1307 USA)。
#
# http://www.opensource.jp/gpl/gpl.ja.html
# ******************************************************

use strict;
use warnings;
use utf8;
use File::Basename;

my $flag_os = 'linux';  # linux/windows
my $flag_charcode = 'utf8';     # utf8/shiftjis


# ユーザの環境に合わせた設定ファイルを読み込む
my $config_filename = dirname($0).'/parse-apache-log.config.pl';
require $config_filename;
##### GLOBAL CONFIG VALUE #####
our $log_directory;
our $log_filename_template;
our $errorlog_filename_template;
our $output_directory;
our $output_filename_template;
our $output_index_filename;
our $error_message_filename;
our $flag_use_zlib;
##### GLOBAL CONFIG VALUE #####

# ユーザディレクトリ下のCPANモジュールを読み込む
use lib ((getpwuid($<))[7]).'/local/cpan/lib/perl5';    # ユーザ環境にCPANライブラリを格納している場合
use lib ((getpwuid($<))[7]).'/local/lib/perl5';         # ユーザ環境にCPANライブラリを格納している場合
use lib ((getpwuid($<))[7]).'/local/lib/perl5/site_perl/5.8.9/mach';         # ユーザ環境にCPANライブラリを格納している場合

use Encode::Guess qw/euc-jp shiftjis iso-2022-jp/;  # 必要ないエンコードは削除すること
use Text::CSV_XS;
use HTTP::Date;
use Time::Local qw( timelocal );
use URI::Escape;
use HTML::Entities;
if($flag_use_zlib == 1) {
    use Compress::Zlib;
}

sub_main();
exit;

sub sub_main {
    # 解析対象年月日
    my $tm = time();    # デフォルト値は現在日時

    # プログラム引数をチェック、解析対象年月日を決定する
    if($#ARGV == 0) {
        # 自動実行モード
        $tm -= int($ARGV[0])*60*60*24;  # 日数前まで遡る
    }
    else {
        # 対話モード
        $tm = sub_user_input();
    }
    
    # 対象ログファイル名、出力ファイル名（フルパス名）の決定
    my ($sec, $min, $hour, $day, $month, $year) = localtime($tm);
    my $log_filename = $log_directory . sprintf($log_filename_template, $year+1900, $month+1, $day);
    my $errorlog_filename = $log_directory . sprintf($errorlog_filename_template, $year+1900, $month+1, $day);
    my $output_filename = $output_directory . sprintf($output_filename_template, $year+1900, $month+1, $day);

    # 対話モードのときは、ターゲットファイル名などを画面表示
    if($#ARGV != 0) {
        printf(" target date         : %04d/%02d/%02d-%02d:%02d:%02d\n".
            " target logfile         : %s\n".
            " target error logfile   : %s\n".
            " output html file       : %s\n",
            $year+1900, $month+1, $day, $hour, $min, $sec,
            $log_filename, $errorlog_filename, $output_filename);
    }
    
    # ログファイルの存在を確認（圧縮されている場合は、解凍）
    my $flag_log_unzip = 0;
    if(-f $log_filename && -r $log_filename) {
        # ログファイルが存在し、読み込み可能な場合：OK
        if($#ARGV != 0) { print("logfile is found.\n"); }
    } elsif(-f $log_filename.'.gz' && !(-f $log_filename)) {
        # ログファイルが存在しないが、圧縮ファイルが存在する場合：解凍する
        sub_extract_gzlog($log_filename);
        $flag_log_unzip = 1;
        if($#ARGV != 0) { print("logfile is extracted from .gz archive.\n"); }
    } else {
        # ログファイルも、圧縮ファイルも見つからない場合：エラーログに出力
        if(-f $log_filename){ sub_write_errorlog('logfile not readable.'); }
        else{ sub_write_errorlog('logfile not found.'); }        
        return;
    }
    
    # エラーログファイルの存在を確認（圧縮されている場合は、解凍）
    my $flag_errorlog_unzip = 0;
    if(-f $errorlog_filename && -r $errorlog_filename) {
        # ログファイルが存在し、読み込み可能な場合：OK
        if($#ARGV != 0) { print("error logfile is found.\n"); }
    } elsif(-f $errorlog_filename.'.gz' && !(-f $errorlog_filename)) {
        # ログファイルが存在しないが、圧縮ファイルが存在する場合：解凍する
        sub_extract_gzlog($errorlog_filename);
        $flag_errorlog_unzip = 1;
        if($#ARGV != 0) { print("error logfile is extracted from .gz archive.\n"); }
    } else {
        # ログファイルも、圧縮ファイルも見つからない場合：エラーログに出力後、続行
        if(-f $errorlog_filename){ sub_write_errorlog('error logfile not readable.'); }
        else{ sub_write_errorlog('error logfile not found.'); }
        # エラーログは存在しなくても、実行を継続する        
    }

    eval {
        # 出力用, レポートファイルを開く
        open(FH_OUT, '>'.$output_filename) or die("Output File open error");
        # 出力文字コードの指定
        binmode(FH_OUT, ":utf8");

        # HTMLファイルの開始部分（<html>〜<body>開始まで）をファイル出力
        sub_output_html_header(*FH_OUT, 'html_head');

        # ログファイルを読み込んで、レポートをHTMLに出力する
        sub_read_logfile($log_filename, *FH_OUT);
        if($#ARGV != 0) { print("report done.\n"); }
        # エラーログファイルを読み込んで、レポートをHTMLに出力する
        if(-r $errorlog_filename) {
            sub_read_errorlogfile($errorlog_filename, *FH_OUT);
            if($#ARGV != 0) { print("error report done.\n"); }
        }

        # HTML構文を閉じて、ファイルを閉じる
        print(FH_OUT "</body>\n</html>\n") or die;
        close(FH_OUT) or die;
    } or do {
        # エラー発生時に、このプログラムのエラーログに出力
        sub_write_errorlog($@);
        exit;
    };


    # ログファイルがgz圧縮されていた場合、解凍したファイルを削除する
    if($flag_log_unzip == 1){
        unlink($log_filename);
        if($#ARGV != 0) { print("uncompressed logfile clean.\n"); }
    }
    # エラーログファイルがgz圧縮されていた場合、解凍したファイルを削除する
    if($flag_errorlog_unzip == 1){
        unlink($errorlog_filename);
        if($#ARGV != 0) { print("uncompressed error logfile clean.\n"); }
    }
    
    # レポートディレクトリの全てのHTMLファイルのリストを作成する（目次HTMLページ）
    sub_make_logfile_index($output_directory, $output_directory.$output_index_filename);
    if($#ARGV != 0) { print("report index done.\n"); }

    return;
}

# 対話モードの時、処理対象年月日をユーザに入力させる
sub sub_user_input {
    my ($sec, $min, $hour, $day, $month, $year) = localtime();
    
    print("parse Apache logfile\n".
        " usage : $0 [day_previous]\n".
        " example : $0 2\n".
        "           (2 days before target file)\n".
        "           $0 0\n".
        "           (today target file)\n".
        "\n".
        "now interactive mode (without script parameter)\n".
        "input target day [".($year+1900)."/".($month+1)."/".$day."] :");
    $_ = <STDIN>;
    chomp();
    unless(length($_)<=0){
        my @arr = split(/\/|\-/, $_);
        if($#arr != 2){ die("date format error") };
        $year = int($arr[0])-1900;
        $month = int($arr[1])-1;
        $day = int($arr[2]);
    }

    my $tm = Time::Local::timelocal($sec, $min, $hour, $day, $month, $year) or die;
    
    return($tm);

}

# ログファイルを読み込んで、エラー行をレポートに出力する
sub sub_read_logfile {
    my $log_filename = shift;       # 引数：ログファイルのファイル名（フルパス）
    local (*FH_OUT) = shift;        # 引数：ファイルハンドル
    my %stat = ();          # 統計データ格納ハッシュ
    sub_stat_log(\%stat, '', 0, 'hash_init');
    my $str_curline = '';   # エラー出力用に現在行を保存

    eval {
        # HTMLファイルのテーブルヘッダ開始部分をファイル出力
        sub_output_html_header(*FH_OUT, 'table_access');

        # CSVファイル形式を、バイナリ対応（日本語対応）、区切り文字はスペースとする
        my $csv = Text::CSV_XS->new({binary=>1, 'sep_char'=>' ', 'escape_char'=>'\\'});

        # 入力用, Apacheログファイルを開く
        open(FH_LOG, '<'.$log_filename) or die("Log File open error");
        # ログファイルを1行ずつ読み込んで順次処理
        while(<FH_LOG>){
            chomp;
            $str_curline = $_;
            if(length($_) < 50){ next; }      # 50文字以下の行は処理しない
            # ログ1行を空白文字で切り分け、それぞれを配列に格納
            # 書式：『127.0.0.1 - - [23/Apr/2011:00:25:14 +0900] "GET /dir/file.html HTTP/1.1" 200 500 "-" "Mozilla/5.0 (X11; U; Linux i686; ja; rv:1.9.2.16) Gecko/20110323 Ubuntu/10.04 (lucid) Firefox/3.6.16" 』
            $csv->parse($_) or next;
            my @arr = $csv->fields() or next;
            if($#arr < 8){ next; }      # 切り分け後のフィールド数が8以下は処理しない
            # 2つに分断された日時フィールドを結合、前後のカギカッコを除去
            my $str_date = $arr[3]." ".$arr[4];
            $str_date =~ s/\[|\]//g;
            # 日時をローカル時間に再構成
            my $tm_logline = HTTP::Date::str2time($str_date) or die;
            my ($sec, $min, $hour, $day, $month, $year) = localtime($tm_logline);
            $year += 1900;
            $month++;
            # HTTPリクエスト文字列をURLデコードし、UTF8に変換
            my $str_httpcmd = HTML::Entities::encode_entities(sub_conv_to_flagged_utf8(URI::Escape::uri_unescape($arr[5])));
            # 統計
            sub_stat_log(\%stat, $str_httpcmd, int($arr[6]), 'stat');
            # HTTPリザルトコード 300以上（WARNING以上）をレポート対象とする
            if(int($arr[6])<300){ next; }
            if(int($arr[6])==304){ next; }  # 304:リダイレクトはレポート対象外
            # HTTPリザルトコードによる背景色の色分け
            my $str_color = '';
            if(int($arr[6])>=300 && int($arr[6])<400){ $str_color = ' style="background-color:#e2edbe;"'; }
            if(int($arr[6])>=400 && int($arr[6])<500){ $str_color = ' style="background-color:#f4e09c;"'; }
            if(int($arr[6])>=500 && int($arr[6])<600){ $str_color = ' style="background-color:#e4bcf1;"'; }
            # 1行 レポート出力
            printf(FH_OUT "<tr%s><td>%04d/%02d/%02d-%02d:%02d:%02d</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
                $str_color,$year,$month,$day,$hour,$min,$sec,$arr[6],$arr[7],$arr[0],$str_httpcmd) or die;
        }
        print(FH_OUT "</table>\n") or die;
        close(FH_LOG) or die;
        # 統計を出力
        print(FH_OUT "<p>拡張子別統計：html=".$stat{'.html'}.", php=".$stat{'.php'}.", cgi=".$stat{'.cgi'}.
            ", jpg=".$stat{'.jpg'}.", png=".$stat{'.png'}.", gif=".$stat{'.gif'}.", pdf=".$stat{'.pdf'}.
            ", lzh=".$stat{'.lzh'}.", zip=".$stat{'.zip'}.", other=".$stat{'.other'}."</p>\n");
        print(FH_OUT "<p>結果コード別統計：200-299=".$stat{'2xx'}.", 300-399=".$stat{'3xx'}.", 400-499=".$stat{'4xx'}.
            ", 500-599=".$stat{'5xx'}."</p>\n");
        print(FH_OUT "<p>ログ全行数：".$stat{'all'}."</p>\n");
    } or do {
        # エラー発生時に、このプログラムのエラーログに出力
        sub_write_errorlog($@);
        exit;
    };
    return;
}

# 統計を取る
sub sub_stat_log {
    my $ref_stat = shift;   # 引数：統計ハッシュ
    my $str = shift;        # 引数：解析文字列（HTTPコマンド文字列）
    my $code = shift;       # 引数：HTTPリザルトコード
    my $flag_mode = shift;  # 引数：機能スイッチ
    
    # 統計用ハッシュを初期化する
    if($flag_mode eq 'hash_init') {
        $$ref_stat{'.html'} = 0;
        $$ref_stat{'.php'} = 0;
        $$ref_stat{'.cgi'} = 0;
        $$ref_stat{'.jpg'} = 0;
        $$ref_stat{'.png'} = 0;
        $$ref_stat{'.gif'} = 0;
        $$ref_stat{'.pdf'} = 0;
        $$ref_stat{'.lzh'} = 0;
        $$ref_stat{'.zip'} = 0;
        $$ref_stat{'.other'} = 0;

        $$ref_stat{'all'} = 0;

        $$ref_stat{'2xx'} = 0;
        $$ref_stat{'3xx'} = 0;
        $$ref_stat{'4xx'} = 0;
        $$ref_stat{'5xx'} = 0;
        return;
    }
    
    # 全体のカウンタ
    $$ref_stat{'all'}++;
    
    # 拡張子別のカウンタ
    my @arr = split(/\s|\?/, $str);
    if($#arr <=1){ $$ref_stat{'.other'}++; }
    else {
        if($arr[1] =~ m/.htm$|.html$|.shtml$/){ $$ref_stat{'.html'}++; }
        elsif($arr[1] =~ m/.php$/){ $$ref_stat{'.php'}++; }
        elsif($arr[1] =~ m/.cgi$/){ $$ref_stat{'.cgi'}++; }
        elsif($arr[1] =~ m/.jpg$|.jpeg$/){ $$ref_stat{'.jpg'}++; }
        elsif($arr[1] =~ m/.png$/){ $$ref_stat{'.png'}++; }
        elsif($arr[1] =~ m/.gif$/){ $$ref_stat{'.gif'}++; }
        elsif($arr[1] =~ m/.pdf$/){ $$ref_stat{'.pdf'}++; }
        elsif($arr[1] =~ m/.lzh$/){ $$ref_stat{'.lzh'}++; }
        elsif($arr[1] =~ m/.zip$|.gz$/){ $$ref_stat{'.zip'}++; }
        else{ $$ref_stat{'.other'}++; }
    }
    
    # HTTPリザルトコード別のカウンタ
    if($code >= 200 && $code <= 299){ $$ref_stat{'2xx'}++; }
    elsif($code >= 300 && $code <= 399){ $$ref_stat{'3xx'}++; }
    elsif($code >= 400 && $code <= 499){ $$ref_stat{'4xx'}++; }
    elsif($code >= 500 && $code <= 599){ $$ref_stat{'5xx'}++; }
    
}

# エラーログファイルを読み込んで、エラー行をレポートに出力する
sub sub_read_errorlogfile {
    my $log_filename = shift;       # 引数：ログファイルのファイル名（フルパス）
    local (*FH_OUT) = shift;        # 引数：ファイルハンドル
    my $str_curline = '';   # エラー出力用に現在行を保存

    eval {
        # HTMLファイルの開始部分（<html>〜<table>開始まで）をファイル出力
        sub_output_html_header(*FH_OUT, 'table_error');

        # 入力用, Apacheログファイルを開く
        open(FH_LOG, '<'.$log_filename) or die("Log File open error");
        # ログファイルを1行ずつ読み込んで順次処理
        while(<FH_LOG>){
            chomp;
            my $str = $_;
            if(length($str)<50){ next; }
            # ログ行をパターンマッチングして分解し $1,$2 ... に格納
            # 書式：『 [Sat Apr 23 00:26:54 2011] [notice] caught SIGTERM, shutting down 』
            $str =~ m/\[\S+\s+(\S+)\s+(\d+)\s+(\S+)\s+(\d{4})\]\s+\[(\w+)\]\s(.*)$/;
            my $datestr = "$2/$1/$4:$3 +0900";
            my $errcode = $5;
            my $msgstr = $6;
            my ($year, $month, $day, $hour, $min, $sec, $tz) = HTTP::Date::parse_date($datestr);

            # HTTPリザルトコードによる背景色の色分け
            my $str_color = '';
            if($errcode eq 'notice'){ $str_color = ' style="background-color:#e2edbe;"'; }
            if($errcode eq 'error'){ $str_color = ' style="background-color:#e4bcf1;"'; }
            # 1行 レポート出力
            printf(FH_OUT "<tr%s><td>%04d/%02d/%02d-%02d:%02d:%02d</td><td>%s</td><td>%s</td></tr>\n",
                $str_color,$year,$month,$day,$hour,$min,$sec,$errcode,$msgstr) or die;
        }
        close(FH_LOG) or die;
        print(FH_OUT "</table>\n");
    } or do {
        # エラー発生時に、このプログラムのエラーログに出力
        sub_write_errorlog($@);
        exit;
    };
    return;
}


# HTMLヘッダ、TABLEヘッダを出力する
sub sub_output_html_header {
    local (*FH) = shift;        # 引数：ファイルハンドル
    my $flag_output_mode = shift;  # 引数：出力モード（'html_head','table_access','table_error'）

    my ($sec, $min, $hour, $day, $month, $year) = localtime();
    my $date_str = sprintf("%04d/%02d/%02d-%02d:%02d:%02d",$year+1900,$month+1,$day, $hour, $min, $sec);

    eval {
        if($flag_output_mode eq 'html_head') {
            print(FH "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n".
                "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"ja\" lang=\"ja\">\n".
                "<head>\n".
                "   <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n".
                "   <meta http-equiv=\"Content-Language\" content=\"ja\" />\n".
                "   <title>Apache log report (".$date_str.")</title>\n".
                "</head>\n".
                "<body style=\"font-size:14px;\">\n".
                "<p>Apache log report</p>\n".
                "<p>Date : ".$date_str."</p>\n") or die;
        } elsif ($flag_output_mode eq 'table_access') {
            print(FH "<p><a href=\"$output_index_filename\">レポートのインデックスページに戻る</a></p>\n".
                "<table style=\"border-color: rgb(0,0,0); border-collapse: collapse; font-size: 12px;\" border=\"1\" cellpadding=\"0\" cellspacing=\"1\">\n".
                "<tr><td>日時</td><td>code</td><td>time</td><td>アクセス元</td><td>HTTP リクエスト コマンド</td></tr>\n") or die;
        } elsif ($flag_output_mode eq 'table_error') {
            print(FH "<p><a href=\"$output_index_filename\">レポートのインデックスページに戻る</a></p>\n".
                "<table style=\"border-color: rgb(0,0,0); border-collapse: collapse; font-size: 12px;\" border=\"1\" cellpadding=\"0\" cellspacing=\"1\">\n".
                "<tr><td>日時</td><td>code</td><td>エラーメッセージ</td></tr>\n") or die;
        }
    } or do {
        # エラー発生時に、このプログラムのエラーログに出力
        sub_write_errorlog($@);
        exit;
    };

}

# レポートファイルの存在するディレクトリ内htmlファイルの一覧を作成する
sub sub_make_logfile_index {
    my $scan_directory = shift;
    my $index_filename = shift;

    eval {
        # ディレクトリ内のHTMLファイルを検索し、パス名を配列に格納
        my @arr_files = glob($scan_directory.'*.html');
        open(FH, '>'.$index_filename) or die;
        # 出力文字コードの指定
        binmode(FH, ":utf8");
        # HTMLファイルの開始部分（<html>〜<body>開始まで）をファイル出力
        sub_output_html_header(*FH, 'html_head');
        print(FH "<p><a href=\"$error_message_filename\">エラー出力を表示する</a></p>\n") or die;
        # 見つかったHTMLファイル名1個ずつを出力処理
        foreach(@arr_files){
            # ディレクトリ名部分を除去
            $_ =~ s/^$scan_directory//;
            # A LINK形式で出力
            printf(FH "<p><a href=\"%s\">%s</a></p>\n",$_,$_) or die;
        }
        print(FH "</body>\n</html>\n");
        close(FH);
    } or do {
        # エラー発生時に、このプログラムのエラーログに出力
        sub_write_errorlog($@);
        exit;
    };

}

# gz圧縮ファイルを（同一ディレクトリ内に）解凍する
sub sub_extract_gzlog {
    my $log_filename = shift;   # 引数：ログファイル名（解凍後のファイル名）
    my $log_filename_gz = $log_filename . '.gz';    # gz圧縮ファイル名

    eval {
        if($flag_use_zlib == 0) {
            # シェルを起動して gunzip コマンドを用いて解凍
            system("sh -c 'gunzip -c $log_filename_gz > $log_filename'");
            unless( -f $log_filename ){ die("sh -c gunzip error, no file created. "); }
        } else {
            # Compress::Zlib ライブラリを用いて解凍
            my $gz = Compress::Zlib::gzopen($log_filename_gz, "rb") or die;
            my $buffer;
            open(FH, '>'.$log_filename) or die;
            while($gz->gzread($buffer)){
                print(FH $buffer) or die;
            }
            close(FH) or die;
            $gz->gzclose() or die;
        }
    } or do {
        # エラー発生時に、このプログラムのエラーログに出力
        sub_write_errorlog($@);
        exit;
    };
}

# エラーログファイルに1行出力する
sub sub_write_errorlog {
    my $str_msg = shift;    # 引数：エラー文字列
    chomp($str_msg);
    open(FH_MSG, '>>'.$output_directory.$error_message_filename) or die;
    my ($sec, $min, $hour, $day, $month, $year) = localtime();
    printf(FH_MSG "%04d/%02d/%02d-%02d:%02d:%02d,%s\n", $year+1900, $month+1, $day, $hour, $min, $sec, $str_msg) or die;
    close(FH_MSG) or die;
    if($#ARGV != 0) { print($str_msg."\n"); }
    return;
}

#################################
# 任意の文字コードの文字列を、UTF-8フラグ付きのUTF-8に変換する
sub sub_conv_to_flagged_utf8{
    my $str = shift;
    my $enc_force = undef;
    if(@_ >= 1){ $enc_force = shift; }      # デコーダの強制指定

    if(!defined($str) || $str eq ''){ return(''); }     # $strが存在しない場合
    if(Encode::is_utf8($str)){ return($str); }  # 既にflagged utf-8に変換済みの場合
    
    # デコーダが強制的に指定された場合
    if(defined($enc_force)){
        if(ref($enc_force)){
            $str = $enc_force->decode($str);
            return($str);
        }
        elsif($enc_force ne '')
        {
            $str = Encode::decode($enc_force, $str);
        }
    }

    my $enc = Encode::Guess->guess($str);   # 文字列のエンコードの判定

    unless(ref($enc)){
        # エンコード形式が2個以上帰ってきた場合 （shiftjis or utf8）
        my @arr_encodes = split(/ /, $enc);
        if(grep(/^$flag_charcode/, @arr_encodes) >= 1){
            # $flag_charcode と同じエンコードが検出されたら、それを優先する
            $str = Encode::decode($flag_charcode, $str);
        }
        elsif(lc($arr_encodes[0]) eq 'shiftjis' || lc($arr_encodes[0]) eq 'euc-jp' || 
            lc($arr_encodes[0]) eq 'utf8' || lc($arr_encodes[0]) eq 'us-ascii'){
            # 最初の候補でデコードする
            $str = Encode::decode($arr_encodes[0], $str);
        }
    }
    else{
        # UTF-8でUTF-8フラグが立っている時以外は、変換を行う
        unless(ref($enc) eq 'Encode::utf8' && utf8::is_utf8($str) == 1){
            $str = $enc->decode($str);
        }
    }

    return($str);
}


# 任意の文字コードの文字列を、UTF-8フラグ無しのUTF-8に変換する
sub sub_conv_to_unflagged_utf8{
    my $str = shift;

    # いったん、フラグ付きのUTF-8に変換
    $str = sub_conv_to_flagged_utf8($str);

    return(Encode::encode('utf8', $str));
}


# UTF8から現在のOSの文字コードに変換する
sub sub_conv_to_local_charset{
    my $str = shift;

    # UTF8から、指定された（OSの）文字コードに変換する
    $str = Encode::encode($flag_charcode, $str);
    
    return($str);
}
