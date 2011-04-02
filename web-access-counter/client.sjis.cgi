#!/usr/bin/perl

use strict;
use warnings;
use CGI;
use DBI;

# ミリ秒単位で、処理時間を求める
use Time::HiRes;

#use URI::Escape;
use Encode;
use Encode::Guess;

# 正常時もエラーログを記録する
my $flag_debugcsv_force_on = 0;

# SQLサーバのDSN定義
my $str_dsn_accesslog = 'DBI:SQLite:dbname=/home/tmg1136-inue2/www/blog/cgi-bin/counter/data/accdb.sqlite';
my $str_dsn_counter = 'DBI:SQLite:dbname=/home/tmg1136-inue2/www/blog/cgi-bin/counter/data/counter.sqlite';

# デバッグ用エラーログファイル
my $str_filepath_debugcsv = "/home/tmg1136-inue2/www/blog/cgi-bin/counter/debuglog.csv";

# ダミーGIF画像ファイル
my $str_filepath_dummygif = 'dummy_img.gif';

# *************
# CGI counter ver 2.0 (C) 2000-2001 INOUE. Hirokazu
# inoue-h@iname.com
# GIF/Text カウンタ、proxy,IP,OS,ブラウザ種別集計とログ記録、曜日・時刻別集計

# *************
# 使用法
#   wwwcount.cgi?test		状況レポート
#   wwwcount.cgi?text		カウンタをテキストで表示
#   wwwcount.cgi?gif		$str_filepath_dummygifで指定されたGIF画像を表示（1x1のダミー画像を想定）
#   wwwcount.cgi			$flag_mode_paramで指定された textまたはgifの動作を行う
# *************

# *************
# 動作モードの設定

# SSIのテキストモードで使用する場合は、$flag_mode_param = "text";
my $flag_mode_param = "text";

# 表示桁数 （0:不要なゼロをつけない）
my $sw_cnt_colmn = 6;

# ロックに使う作業用変数
my $flag_is_locked = 0;			# ロックされている場合 =1
my $tm_lock = 0;			# ロック時刻 （UNIX秒）
my $tm_lock_timeout = 15;		# ロックのタイムアウト秒数

# 同じアドレスからのアクセスをカウントしない (1:ON, 0:OFF)
my $flag_ip_sameaddr_check = 1;
# 作業用変数（同一アドレスからのアクセスの場合1になる）
my $flag_is_ip_sameaddr = 0;

# IPアドレスのホスト名への変換を行う (1:ON, 0:OFF)
my $flag_ip_gethost = 1;

# ログに記録するときに、削除する文字列
my $str_mw_URL = 'http://netlog.jpn.org/mw/index.php?title=';
my @arr_hide_URL = ('http://netlog.jpn.org',
				'http://www.netlog.jpn.org');


# *************
# MAIN

# *************
# グローバル変数
my $strTmp = "";

# 環境変数TZを日本時間に設定する
$ENV{'TZ'} = "JST-9";


# 処理時間計測
my $tmmStart = Time::HiRes::time();

# 環境変数から解析する変数
my $ip = "";			# アクセス元のIPアドレス
my $hostname = "";		# アクセス元のホスト名
my $browser = "";		# ブラウザ変数
my $referer = "";		# 参照元URL
my $lang = "";			# 言語 "us ja ko ..." の集計形式
my $lang_raw = "";		# 言語（生データ）

# カウンタ
my $nCounter = 0;

# *************
# 初期値の決定（これ以後は、操作禁止）

# 異常終了の場合のトラップ
sub sigexit {  sub_write_to_debug_log("===SIGEXIT==="); exit(0); }
$SIG{'PIPE'} = $SIG{'INT'} = $SIG{'HUP'} = $SIG{'QUIT'} = $SIG{'TERM'} = "sigexit";


# アクセスログ用のデータを得る（IPアドレス、リファラー、ブラウザ文字列）
($ip,$hostname,$browser,$referer,$lang) = sub_make_accesslod_data();


# *************
# プログラムの引数により処理を切り替える
if (!defined($ARGV[0]) || $ARGV[0] eq ''){
	# 何も指定しない場合は、初期値の動作を行う
} elsif ($ARGV[0] eq "text") {
	$flag_mode_param = "text";		# SSIモード（テキストでカウンタ値を返す）
} elsif ($ARGV[0] eq "gif") {
	$flag_mode_param = "gif";		# ブランクのGIF画像を返す（ダミー画像）
} elsif ($ARGV[0] eq 'test') {
	$flag_mode_param = "test";		# カウンタのステータス表示（テキスト）
} else {
	# その他のオプションを指定した場合、エラー終了
	sub_write_to_debug_log("===ARGV_ERROR===");
	exit;
}


# カウンタデータベースを読み込む（ロック設定、IP重複判定）
if($flag_mode_param eq "test") {
	($flag_is_locked, $nCounter, $flag_is_ip_sameaddr, $tm_lock) = sub_read_counter_db($ip, 0);
} else {
	($flag_is_locked, $nCounter, $flag_is_ip_sameaddr, $tm_lock) = sub_read_counter_db($ip, 1);
}

# *************
# CGIが使用できるかテストを行う。
if ($flag_mode_param eq "test") {
	sub_test_cgi($ip,$hostname,$browser,$referer,$lang, $tm_lock, $flag_is_locked, $flag_is_ip_sameaddr, $nCounter);
	exit;
}


# *************
# ロックが掛かっていない場合、アクセスログ記録を行う
if($flag_is_locked == 0)
{
	# 次の2つの条件のどちらかの時にログを記録する
	# 1. $flag_ip_sameaddr_check==0のとき
	# 2. $flag_ip_sameaddr_check==1かつ$nSameIp==0のとき
	if($flag_is_ip_sameaddr == 0 || $flag_ip_sameaddr_check == 0)
	{
		# アクセスログDBに追記
		sub_write_accesslog_db($ip,$hostname,$browser,$referer,$lang);
		
		$nCounter++;	# カウンターを1増やす
		
		sub_write_counter_db($ip, $nCounter);		# カウンタ書き込み、ロック解除
	}
}
else
{	# ロックが掛かっていた場合はデバッグログに書き込む
	sub_write_to_debug_log("===LOCK===");
}


# ミリ秒単位での処理開始からの経過時間
my $tmmLapse = Time::HiRes::time() - $tmmStart;

# 処理時間が1秒以上かかった場合は、デバッグログに書き込む
if($tmmLapse > 1) {
	sub_write_to_debug_log("===LONGTIME===");
}
elsif($flag_debugcsv_force_on == 1){
	# デバッグ用に、全てのアクセスでデバッグログに書き込む
	sub_write_to_debug_log("===DEBUG===");
}


# GIFまたはテキストでカウンターを描画する
sub_display_counter($nCounter, $flag_mode_param, $sw_cnt_colmn);


exit;
# CGI カウンター 処理終了
# *************

# *************
# HTML でメッセージを出力する
#
# 用例 HTML_Output(string)
sub HTML_Output {
	my $str = shift;	# 引数
	my $q = new CGI;

	print $q->header(-type=>'text/html', -charset=>'shift_jis');
	print $q->start_html(-title=>"Access log query",-lang=>'ja-JP');
	print($str);
	print $q->end_html;
}

# *************
# 画面表示（テキストカウンタ or GIFダミー画像）
# sub_display_counter($n_counter, $flag_mode, $n_counter_digit)
sub sub_display_counter{
	# 引数
	my $n_counter = shift;
	my $flag_mode = shift;
	my $n_counter_digit = shift;
	# 引数チェック
	if(!defined($n_counter) || $n_counter eq '' || $n_counter<1){ $n_counter = 0; }
	if(!defined($flag_mode) || $flag_mode eq ''){ $flag_mode = 'null'; }
	if(!defined($n_counter_digit) || $n_counter_digit eq '' || $n_counter_digit<1){ $n_counter_digit = 4; }


	if ($flag_mode eq "text") {
		# テキストでカウンターを描画
		# 表示桁数に合わせて、現在のカウンタ値を文字列にする
		my $str = sprintf(sprintf("%%0%dld", $n_counter_digit), $n_counter);
		print("Content-type: text/html\n\n");
		print($str."\n");
		return;
	} elsif ($flag_mode eq "gif") {
		# GIF のダミー画像を表示する
		unless( -f $str_filepath_dummygif ){ return; }
		print("Content-type: image/gif\n\n");
		my $n_filesize = -s $str_filepath_dummygif;
		open(IN, "<$str_filepath_dummygif");
		binmode(IN);
		binmode(STDOUT);
		my $buffer = undef;
		read(IN, $buffer, $n_filesize);
		close(IN);
		print $buffer;
		return;
	}

}


# *************
# SQL格納に有害になる文字を除去またはエンコードする
# sub_conv_to_safe_str(string, maxlength);
sub sub_conv_to_safe_str
{
	# 引数
	my $str = shift;
	my $n_max_length = shift;
	# 引数チェック
	if(!defined($str) || length($str)<=0){ $str = ''; }
	if(!defined($n_max_length) || $n_max_length<=0){ $n_max_length = 255; }

	# 任意のエンコード → Shift JIS に変換
	my $enc = guess_encoding($str, qw/euc-jp shiftjis 7bit-jis/);
	if(!ref($enc))
	{	# UTF8は常に判定エラーになるため
		$str = encode('shiftjis', Encode::decode('utf8', $str));
	}
	else
	{
		$str = encode('shiftjis', decode($enc->name, $str));
	}

	# バイナリや特殊記号を削除（SQLインジェクション防止）
	# （なお、0x5cのバックスラッシュはSJISの文字コードを侵食するため削除しない
	$str =~ tr/(\x00-\x1f|\x21-\x27)//;
	$str =~ s/(\x2c|\x3b|<|>|\x3f|\x60)//g;


	# URLエンコード
#	$strTmp = uri_escape($strTmp);
	# シングルクオート（'）等が標準ではエンコードされないことの対処
#	$strTmp = uri_escape($strTmp, "'");

	# URLエンコードを最小限だけ行う		（上の処理で置換済みのためこの処理不要）
#	$strTmp = uri_escape($strTmp, "%|");

	# 文字列を最大長さで切り捨てる
	$str = substr($str, 0, $n_max_length);
	
	return $str;
}

# *************
# アクセスログ用のデータを生成する
# array($ip,$hostname,$browser,$referer,$lang) = sub_make_accesslod_data()
sub sub_make_accesslod_data
{
	# アクセス元のIPアドレス
	my $ip = $ENV{'REMOTE_ADDR'};
	if(!defined($ip) || length($ip)<=0){ $ip = '0.0.0.0'; }
	
	$ip = sub_conv_to_safe_str($ip, 255);

	# アクセス元のホスト名
	my $hostname = $ENV{'REMOTE_HOST'};
	if(!defined($hostname) || length($hostname)<=0){ $hostname = ''; }
	if ($flag_ip_gethost && (($hostname eq "") || ($hostname eq $ip))) {
		if($ip eq '127.0.0.1' || $ip eq ''){ $hostname = 'localhost'; }
		elsif($ip eq '0.0.0.0'){ $hostname = ''; }
		else{ $hostname = gethostbyaddr(pack("C4", split(/\./, $ip)), 2); }
	}
	
	$hostname = sub_conv_to_safe_str($hostname, 255);

	# ブラウザ名（ユーザエージェント名）
	my $browser = $ENV{'HTTP_USER_AGENT'};
	if(!defined($browser) || length($browser)<=0){ $browser = ''; }

	$browser = sub_conv_to_safe_str($browser, 255);

	# 呼び出された元のホームページ名（リファラー）
	my $referer = $ENV{'HTTP_REFERER'};
	if(!defined($referer) || length($referer)<=0){ $referer = ''; }
	if($referer eq "" && defined($ENV{'REQUEST_URI'}) &&  $ENV{'REQUEST_URI'} ne "") { $referer = $ENV{'REQUEST_URI'};}
	# 格納できない文字を処理する
	$referer =~ s/%([0-9a-fA-F][0-9a-fA-F])/pack("C", hex($1))/eg;
	$referer =~ s/\\//g;
	$referer =~ s/\xa//eg;
	$referer =~ s/,//eg;

	# URLのドメイン部分を除去して、文字列長を短くする
	if($referer =~ $str_mw_URL) {
		$referer =~ s/$str_mw_URL/mw:/;
		Encode::from_to($referer, 'utf8', 'sjis');
	}
	foreach(@arr_hide_URL)
	{
		my $str = $_;
		$referer =~ s/$str//;
	}

	$referer = sub_conv_to_safe_str($referer, 255);


	# インストール言語
	my $lang = '';
	my $lang_raw = $ENV{'HTTP_ACCEPT_LANGUAGE'};
	if(!defined($lang_raw) || length($lang_raw)<=0){ $lang_raw = ''; }
	if($lang_raw =~ "en") { $lang = $lang . "en ";}
	if($lang_raw =~ "ja") { $lang = $lang . "ja ";}
	if($lang_raw =~ "zh") { $lang = $lang . "zh ";}
	if($lang_raw =~ "ko") { $lang = $lang . "ko ";}
	if($lang_raw =~ "fr") { $lang = $lang . "fr ";}
	if($lang_raw =~ "de") { $lang = $lang . "de ";}
	if($lang_raw =~ "es") { $lang = $lang . "es ";}
	if($lang_raw =~ "pt") { $lang = $lang . "pt ";}		# arabian
	if($lang_raw =~ "el") { $lang = $lang . "el ";}		# greek
	if($lang_raw =~ "it") { $lang = $lang . "it ";}
	if($lang_raw =~ "ru") { $lang = $lang . "ru ";}
	if($lang_raw =~ "ar") { $lang = $lang . "ar ";}		# arabian
	if($lang_raw =~ "nl") { $lang = $lang . "nl ";}
	if($lang_raw =~ "ro") { $lang = $lang . "ro ";}
	if($lang_raw =~ "hu") { $lang = $lang . "ku ";}
	if($lang_raw =~ "cs") { $lang = $lang . "cs ";}
	if($lang_raw =~ "pl") { $lang = $lang . "pl ";}

	return($ip,$hostname,$browser,$referer,$lang);

}

# *************
# カウンターDB読み込み（カウンター値、ロック掛ける、同一IPチェック）
# sub_read_counter_db($ip, $flag_lock_mode)
sub sub_read_counter_db
{
	# 引数
	my $ip = shift;	# IPアドレス
	my $flag_lock_mode = shift;	# ロック機能を使う場合、1
	# 引数チェック
	if(!defined($ip) || $ip eq ''){ $ip = '0.0.0.0'; }
	if(!defined($flag_lock_mode) || $flag_lock_mode != 1 || $flag_lock_mode != 0){ $flag_lock_mode = 0; }

	# 現在日時
	my $tm_now = time();

	my $nLocked = 1;	# 1:ロック、0:正常
	my $nCounter = 0;
	my $nSameIp = 0;	# 1:DBに登録されているIP、 0:DBに登録されていない

	my $tm_lock = 0;		# 読み出したロック時間
	
	my $dbh = undef;
	eval{
		# SQLサーバに接続
		$dbh = DBI->connect($str_dsn_counter, "", "", {PrintError => 0, AutoCommit => 1}) or die(DBI::errstr);

		# ロック状況を得る
		my $str_query = "SELECT tm_epock FROM locktbl WHERE idx = '0'";
		my $sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);
		my @row = $sth->fetchrow_array();
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# 再利用するためundef

		if(@row){ $tm_lock = $row[0]; }
		else{ $tm_lock = 0; }		# 値がない場合は UNIX時刻 0 とする
		@row = ();	# 再利用するため配列初期化

		if($tm_now - $tm_lock > $tm_lock_timeout){ $nLocked = 0; }
		else{ $nLocked = 1;}

		# 現在のカウンター値を得る
		$str_query = "SELECT counter FROM countertbl WHERE idx = '0'";
		$sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);
		@row = $sth->fetchrow_array();
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# 再利用するためundef
		
		if(@row){ $nCounter = $row[0]; }
		else{ $nCounter = 0; }
		@row = ();	# 再利用するため配列初期化

		# 指定されたIPアドレスが既に登録されているかチェックする
		$str_query = "SELECT COUNT (*) FROM iptbl WHERE ip = ?";
		$sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute($ip) or die(DBI::errstr);
		@row = $sth->fetchrow_array();
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# 再利用するためundef
		
		if(@row && $row[0] > 0){ $nSameIp = 1; }
		else{ $nSameIp = 0; }

		if($flag_lock_mode == 1 && $nLocked == 0)	# ロックが掛かっていない場合
		{
			# 次の2つの条件のどちらかの時にログを記録する
			# 1. ロックされておらず、$flag_ip_sameaddr_check==0のとき
			# 2. ロックされておらず、$flag_ip_sameaddr_check==1かつ$nSameIp==0のとき
			if($flag_ip_sameaddr_check == 0 || $nSameIp == 0)
			{
				$str_query = "UPDATE locktbl SET tm_epock = ? WHERE idx = '0'";
				$sth = $dbh->prepare($str_query) or die(DBI::errstr);
				$sth->execute($tm_now) or die(DBI::errstr);
				
				$tm_lock = $tm_now;
			}
		}
		

		# DBを閉じる
		$dbh->disconnect() or die(DBI::errstr);
	};
	if($@){
		# evalによるDBエラートラップ：エラー時の処理
		if(defined($dbh)){ $dbh->disconnect(); }
		my $str = $@;
		chomp($str);
		sub_write_to_debug_log("===SQLERROR_COUNTERDB_READ (".$str.")===");
	}
	
	return($nLocked, $nCounter, $nSameIp, $tm_lock);
}

# *************
# カウンターDBアップデート（カウンター書き込み、ロック解除、IP書き込み）
# sub_write_counter_db($ip, $n_counter)
sub sub_write_counter_db
{
	# 引数
	my $ip = shift;
	my $n_counter = shift;
	# 引数チェック
	if(!defined($ip) || $ip eq ''){ $ip = '0.0.0.0'; }
	if(!defined($n_counter) || $n_counter eq '' || $n_counter <= 0){ $n_counter = 0; }

	# 現在日時
	my $tm_now = time();

	my $n_index = 0;	# アクセス元ipアドレスリストのインデックス

	my $dbh = undef;
	eval{
		# SQLサーバに接続
		$dbh = DBI->connect($str_dsn_counter, "", "", {PrintError => 0, AutoCommit => 1}) or die(DBI::errstr);

		# 現在のカウンター値を書き込む
		my $str_query = "UPDATE countertbl SET counter = ? WHERE idx = '0'";
		my $sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute($n_counter) or die(DBI::errstr);
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# 再利用するためundef

		# 最も古い時間の「指定IPアドレス」行のインデックスを取り出す
#			$str_query = "SELECT idx FROM iptbl ORDER BY tm_epock LIMIT 1";
		$str_query = "SELECT idx FROM iptbl WHERE ip = ? ORDER BY tm_epock LIMIT 1";
		$sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute($ip) or die(DBI::errstr);
		my @row = $sth->fetchrow_array();
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# 再利用するためundef
		
		if(@row)
		{	# IPアドレスが既に登録されていた場合
			$n_index = $row[0];
		}
		else
		{
			@row = ();	# 再利用するため配列初期化
			# 最も古い時間のインデックスを取り出す
			$str_query = "SELECT idx FROM iptbl ORDER BY tm_epock LIMIT 1";
			$sth = $dbh->prepare($str_query) or die(DBI::errstr);
			$sth->execute() or die(DBI::errstr);
			@row = $sth->fetchrow_array();
			$sth->finish() or die(DBI::errstr);
			$sth = undef;	# 再利用するためundef
			
			if(@row) { $n_index = $row[0]; }
			else { $n_index = 0; }
		}

		# IPアドレス、現在時刻を書き込む
		$str_query = "UPDATE iptbl SET ip = ?, tm_epock = ? WHERE idx = ?";

		$sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute($ip, $tm_now, $n_index) or die(DBI::errstr);
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# 再利用するためundef

		# ロック解除
		$str_query = "UPDATE locktbl SET tm_epock = '0' WHERE idx = '0'";
		$sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);
		$sth->finish() or die(DBI::errstr);
		

		# DBを閉じる
		$dbh->disconnect() or die(DBI::errstr);
	};
	if($@){
		# evalによるDBエラートラップ：エラー時の処理
		if(defined($dbh)){ $dbh->disconnect(); }
		my $str = $@;
		chomp($str);
		sub_write_to_debug_log("===SQLERROR_COUNTERDB_UPDATE (".$str.")===");
	}
	
	return;
}


# *************
# 重複判定用IPアドレスリストを表示用に読み出す（テストモード用）
sub sub_textout_iplist
{
	my $str = "<p>info : IP address list in counter DB<br />\n";
	
	my $dbh = undef;
	eval{
		# SQLサーバに接続
		$dbh = DBI->connect($str_dsn_counter, "", "", {PrintError => 0, AutoCommit => 1}) or die(DBI::errstr);

		# IPアドレスリストを全て読み出す
		my $str_query = "SELECT * FROM iptbl ORDER BY tm_epock";
		my $sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);

		my @row;
		while(@row = $sth->fetchrow_array())
		{
			$str .= sprintf("&nbsp;&nbsp;idx=%02d, ip=%-15s, time=%d/%02d/%02d %02d:%02d:%02d<br />\n", $row[0], $row[1], (localtime($row[2]))[5]+1900, (localtime($row[2]))[4]+1, (localtime($row[2]))[3], (localtime($row[2]))[2], (localtime($row[2]))[1], (localtime($row[2]))[0]);
		}

		$sth->finish() or die(DBI::errstr);
		# DBを閉じる
		$dbh->disconnect() or die(DBI::errstr);
	};
	if($@){
		# evalによるDBエラートラップ：エラー時の処理
		if(defined($dbh)){ $dbh->disconnect(); }
		my $str = $@;
		chomp($str);
		sub_write_to_debug_log("===SQLERROR_COUNTERDB_READ (".$str.")===");
	}

	$str .= "</p>\n";
	
	return($str);
}

# *************
# アクセスログデータベースに、ログを１行書き込む
# sub_write_accesslog_db($ip,$hostname,$browser,$referer,$lang)
sub sub_write_accesslog_db
{
	# 引数
	my $ip = shift;
	my $hostname = shift;
	my $browser = shift;
	my $referer = shift;
	my $lang = shift;
	# 引数チエック
	if(!defined($ip) || $ip eq ''){ $ip = ''; }
	if(!defined($hostname) || $hostname eq ''){ $hostname = ''; }
	if(!defined($browser) || $browser eq ''){ $browser = ''; }
	if(!defined($referer) || $referer eq ''){ $referer = ''; }
	if(!defined($lang) || $lang eq ''){ $lang = ''; }

	# 現在日時
	my $tm_now = time();

	# 日時
	my ($year,$month,$day,$hour,$min,$sec,$n_weekday,$n_day_second) = sub_make_datetime_str();

	my $dbh = undef;
	eval{
		# DBに接続
		$dbh = DBI->connect($str_dsn_accesslog, "", "", {PrintError => 0, AutoCommit => 1}) or die(DBI::errstr);
		# クエリ作成
		my $str_query = "insert into acctbl values(null,?,?,?,?,?,?,?,?)";
		my $sth = $dbh->prepare($str_query) or die(DBI::errstr);

		# クエリ命令発行
		$sth->execute($tm_now, $n_weekday, $n_day_second, $ip, $hostname, $browser, $referer, $lang) or die(DBI::errstr);
		$sth->finish() or die(DBI::errstr);

		# DBを閉じる
		$dbh->disconnect() or die(DBI::errstr);
		
	};
	if($@){
		# evalによるDBエラートラップ：エラー時の処理
		if(defined($dbh)){ $dbh->disconnect(); }
		my $str = $@;
		chomp($str);
		sub_write_to_debug_log("===SQLERROR_ACCDB (".$str.")===");
	}

	return;
}


# *************
# テストモード（情報表示）
# sub_test_cgi($ip,$hostname,$browser,$referer,$lang, $tm_lock, $flag_lock, $flag_sameip, $n_counter);
sub sub_test_cgi
{
	# 引数
	my $ip = shift;
	my $hostname = shift;
	my $browser = shift;
	my $referer = shift;
	my $lang = shift;
	my $tm_lock = shift;
	my $flag_lock = shift;
	my $flag_sameip = shift;
	my $n_counter = shift;
	
	# 現在日時
	my $tm_now = time();

	my $str = "<p>=== counter and access log program diagnostic ===</p>\n";

	if($flag_lock > 0) { $str .= "<p>error : process locked</p>\n";}
	else { $str .= "<p>info : lock status clear, OK</p>\n";}
	$str .= sprintf("<p>info : lock time %d/%02d/%02d %02d:%02d:%02d</p>\n", (localtime($tm_lock))[5]+1900, (localtime($tm_lock))[4]+1, (localtime($tm_lock))[3], (localtime($tm_lock))[2], (localtime($tm_lock))[1], (localtime($tm_lock))[0]);
	$str .= sprintf("<p>info : current time %d/%02d/%02d %02d:%02d:%02d</p>\n", (localtime($tm_now))[5]+1900, (localtime($tm_now))[4]+1, (localtime($tm_now))[3], (localtime($tm_now))[2], (localtime($tm_now))[1], (localtime($tm_now))[0]);
	if($flag_sameip > 0) { $str .= "<p>info : IP address ".$ip." is listed in DB, OK</p>\n";}
	else { $str .= "<p>info : IP address ".$ip." is NOT listed in DB, OK</p>\n";}
	$str .= "<p>info : current counter = ".$n_counter.", OK</p>\n";

	my $dbh = undef;
	eval{
		# SQLサーバに接続
		$dbh = DBI->connect($str_dsn_accesslog, "", "", {PrintError => 1, AutoCommit => 0}) or die(DBI::errstr);

		# アクセスデータベース全体のデータ数
		my $str_query = "SELECT COUNT (*) FROM acctbl";
		my $sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);
		my @row = $sth->fetchrow_array();
		$sth->finish() or die(DBI::errstr);

		$str .= "<p>info : Access Log DB ".$row[0]." lines exist, OK</p>\n";
		$dbh->disconnect() or die(DBI::errstr);

	};
	if($@){
		# evalによるDBエラートラップ：エラー時の処理
		if(defined($dbh)){ $dbh->disconnect(); }
		my $str = $@;
		chomp($str);
		sub_write_to_debug_log("===SQLERROR_ACCDB (".$str.")===");
		$str .= "<p>DBI access error<br />\n";
	}
	
	
	$str .= "<p>ip = ".$ip."<br />\n";
	$str .= "hostname = ".$hostname."<br />\n";
	$str .= "browser = ".$browser."<br />\n";
	$str .= "referer = ".$referer."</p>\n";

	$str .= &sub_textout_iplist();

	HTML_Output($str);
	
	return();
}

# *************
# エラー時のデバッグログCSVファイルへの書き込み
# sub_write_to_debug_log($str)
sub sub_write_to_debug_log
{
	my $str = shift;

	# 現在日時
	my $tm_now = time();

	# ミリ秒単位での処理開始からの経過時間
	my $str_millitime_lapse = Time::HiRes::time() - $tmmStart;
	# 現在日時
	my $str_time_now .= sprintf("%d/%02d/%02d,%02d:%02d:%02d", (localtime($tm_now))[5]+1900, (localtime($tm_now))[4]+1, (localtime($tm_now))[3], (localtime($tm_now))[2], (localtime($tm_now))[1], (localtime($tm_now))[0]);
	# ロック日時
	my $str_time_lock .= sprintf("%d/%02d/%02d,%02d:%02d:%02d", (localtime($tm_lock))[5]+1900, (localtime($tm_lock))[4]+1, (localtime($tm_lock))[3], (localtime($tm_lock))[2], (localtime($tm_lock))[1], (localtime($tm_lock))[0]);

	# デバッグ用ファイルに書き込む
	open(OUT, ">> $str_filepath_debugcsv");	# 追記モード
	
	print(OUT $str_time_now.",lock=".$str_time_lock.",laps=".$str_millitime_lapse.",argv=".(defined($ARGV[0])?$ARGV[0]:'').",ip=".$ip.", hostname=".$hostname.",browser=".$browser.",referer=".$referer.",lock=".$flag_is_locked.",sameip=".$flag_is_ip_sameaddr);
	if(defined($str) && $str ne '') { print(OUT ",".$str);}
	print(OUT "\n");
	close(OUT);

}

# 日時データの文字列を作成する
# ($year,$month,$day,$hour,$min,$sec,$n_weekday,$n_day_second) = 
#   sub_make_datetime_str()
sub sub_make_datetime_str{

	# 現在日時
	my $tm_now = time();
	# 曜日を求める
	my $n_weekday = (localtime($tm_now))[6];
	my ($sec, $min, $hour, $day, $month, $year) = localtime($tm_now);
	# その日の0時からの秒数
	my $n_day_second = $hour*60*60 + $min*60 + $sec;

	return($year,$month,$day,$hour,$min,$sec,$n_weekday,$n_day_second);

}

# ************* (EOF)
