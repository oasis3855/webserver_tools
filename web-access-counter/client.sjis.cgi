#!/usr/bin/perl

use strict;
use warnings;
use CGI;
use DBI;

# �~���b�P�ʂŁA�������Ԃ����߂�
use Time::HiRes;

#use URI::Escape;
use Encode;
use Encode::Guess;

# ���펞���G���[���O���L�^����
my $flag_debugcsv_force_on = 0;

# SQL�T�[�o��DSN��`
my $str_dsn_accesslog = 'DBI:SQLite:dbname=/home/tmg1136-inue2/www/blog/cgi-bin/counter/data/accdb.sqlite';
my $str_dsn_counter = 'DBI:SQLite:dbname=/home/tmg1136-inue2/www/blog/cgi-bin/counter/data/counter.sqlite';

# �f�o�b�O�p�G���[���O�t�@�C��
my $str_filepath_debugcsv = "/home/tmg1136-inue2/www/blog/cgi-bin/counter/debuglog.csv";

# �_�~�[GIF�摜�t�@�C��
my $str_filepath_dummygif = 'dummy_img.gif';

# *************
# CGI counter ver 2.0 (C) 2000-2001 INOUE. Hirokazu
# inoue-h@iname.com
# GIF/Text �J�E���^�Aproxy,IP,OS,�u���E�U��ʏW�v�ƃ��O�L�^�A�j���E�����ʏW�v

# *************
# �g�p�@
#   wwwcount.cgi?test		�󋵃��|�[�g
#   wwwcount.cgi?text		�J�E���^���e�L�X�g�ŕ\��
#   wwwcount.cgi?gif		$str_filepath_dummygif�Ŏw�肳�ꂽGIF�摜��\���i1x1�̃_�~�[�摜��z��j
#   wwwcount.cgi			$flag_mode_param�Ŏw�肳�ꂽ text�܂���gif�̓�����s��
# *************

# *************
# ���샂�[�h�̐ݒ�

# SSI�̃e�L�X�g���[�h�Ŏg�p����ꍇ�́A$flag_mode_param = "text";
my $flag_mode_param = "text";

# �\������ �i0:�s�v�ȃ[�������Ȃ��j
my $sw_cnt_colmn = 6;

# ���b�N�Ɏg����Ɨp�ϐ�
my $flag_is_locked = 0;			# ���b�N����Ă���ꍇ =1
my $tm_lock = 0;			# ���b�N���� �iUNIX�b�j
my $tm_lock_timeout = 15;		# ���b�N�̃^�C���A�E�g�b��

# �����A�h���X����̃A�N�Z�X���J�E���g���Ȃ� (1:ON, 0:OFF)
my $flag_ip_sameaddr_check = 1;
# ��Ɨp�ϐ��i����A�h���X����̃A�N�Z�X�̏ꍇ1�ɂȂ�j
my $flag_is_ip_sameaddr = 0;

# IP�A�h���X�̃z�X�g���ւ̕ϊ����s�� (1:ON, 0:OFF)
my $flag_ip_gethost = 1;

# ���O�ɋL�^����Ƃ��ɁA�폜���镶����
my $str_mw_URL = 'http://netlog.jpn.org/mw/index.php?title=';
my @arr_hide_URL = ('http://netlog.jpn.org',
				'http://www.netlog.jpn.org');


# *************
# MAIN

# *************
# �O���[�o���ϐ�
my $strTmp = "";

# ���ϐ�TZ����{���Ԃɐݒ肷��
$ENV{'TZ'} = "JST-9";


# �������Ԍv��
my $tmmStart = Time::HiRes::time();

# ���ϐ������͂���ϐ�
my $ip = "";			# �A�N�Z�X����IP�A�h���X
my $hostname = "";		# �A�N�Z�X���̃z�X�g��
my $browser = "";		# �u���E�U�ϐ�
my $referer = "";		# �Q�ƌ�URL
my $lang = "";			# ���� "us ja ko ..." �̏W�v�`��
my $lang_raw = "";		# ����i���f�[�^�j

# �J�E���^
my $nCounter = 0;

# *************
# �����l�̌���i����Ȍ�́A����֎~�j

# �ُ�I���̏ꍇ�̃g���b�v
sub sigexit {  sub_write_to_debug_log("===SIGEXIT==="); exit(0); }
$SIG{'PIPE'} = $SIG{'INT'} = $SIG{'HUP'} = $SIG{'QUIT'} = $SIG{'TERM'} = "sigexit";


# �A�N�Z�X���O�p�̃f�[�^�𓾂�iIP�A�h���X�A���t�@���[�A�u���E�U������j
($ip,$hostname,$browser,$referer,$lang) = sub_make_accesslod_data();


# *************
# �v���O�����̈����ɂ�菈����؂�ւ���
if (!defined($ARGV[0]) || $ARGV[0] eq ''){
	# �����w�肵�Ȃ��ꍇ�́A�����l�̓�����s��
} elsif ($ARGV[0] eq "text") {
	$flag_mode_param = "text";		# SSI���[�h�i�e�L�X�g�ŃJ�E���^�l��Ԃ��j
} elsif ($ARGV[0] eq "gif") {
	$flag_mode_param = "gif";		# �u�����N��GIF�摜��Ԃ��i�_�~�[�摜�j
} elsif ($ARGV[0] eq 'test') {
	$flag_mode_param = "test";		# �J�E���^�̃X�e�[�^�X�\���i�e�L�X�g�j
} else {
	# ���̑��̃I�v�V�������w�肵���ꍇ�A�G���[�I��
	sub_write_to_debug_log("===ARGV_ERROR===");
	exit;
}


# �J�E���^�f�[�^�x�[�X��ǂݍ��ށi���b�N�ݒ�AIP�d������j
if($flag_mode_param eq "test") {
	($flag_is_locked, $nCounter, $flag_is_ip_sameaddr, $tm_lock) = sub_read_counter_db($ip, 0);
} else {
	($flag_is_locked, $nCounter, $flag_is_ip_sameaddr, $tm_lock) = sub_read_counter_db($ip, 1);
}

# *************
# CGI���g�p�ł��邩�e�X�g���s���B
if ($flag_mode_param eq "test") {
	sub_test_cgi($ip,$hostname,$browser,$referer,$lang, $tm_lock, $flag_is_locked, $flag_is_ip_sameaddr, $nCounter);
	exit;
}


# *************
# ���b�N���|�����Ă��Ȃ��ꍇ�A�A�N�Z�X���O�L�^���s��
if($flag_is_locked == 0)
{
	# ����2�̏����̂ǂ��炩�̎��Ƀ��O���L�^����
	# 1. $flag_ip_sameaddr_check==0�̂Ƃ�
	# 2. $flag_ip_sameaddr_check==1����$nSameIp==0�̂Ƃ�
	if($flag_is_ip_sameaddr == 0 || $flag_ip_sameaddr_check == 0)
	{
		# �A�N�Z�X���ODB�ɒǋL
		sub_write_accesslog_db($ip,$hostname,$browser,$referer,$lang);
		
		$nCounter++;	# �J�E���^�[��1���₷
		
		sub_write_counter_db($ip, $nCounter);		# �J�E���^�������݁A���b�N����
	}
}
else
{	# ���b�N���|�����Ă����ꍇ�̓f�o�b�O���O�ɏ�������
	sub_write_to_debug_log("===LOCK===");
}


# �~���b�P�ʂł̏����J�n����̌o�ߎ���
my $tmmLapse = Time::HiRes::time() - $tmmStart;

# �������Ԃ�1�b�ȏォ�������ꍇ�́A�f�o�b�O���O�ɏ�������
if($tmmLapse > 1) {
	sub_write_to_debug_log("===LONGTIME===");
}
elsif($flag_debugcsv_force_on == 1){
	# �f�o�b�O�p�ɁA�S�ẴA�N�Z�X�Ńf�o�b�O���O�ɏ�������
	sub_write_to_debug_log("===DEBUG===");
}


# GIF�܂��̓e�L�X�g�ŃJ�E���^�[��`�悷��
sub_display_counter($nCounter, $flag_mode_param, $sw_cnt_colmn);


exit;
# CGI �J�E���^�[ �����I��
# *************

# *************
# HTML �Ń��b�Z�[�W���o�͂���
#
# �p�� HTML_Output(string)
sub HTML_Output {
	my $str = shift;	# ����
	my $q = new CGI;

	print $q->header(-type=>'text/html', -charset=>'shift_jis');
	print $q->start_html(-title=>"Access log query",-lang=>'ja-JP');
	print($str);
	print $q->end_html;
}

# *************
# ��ʕ\���i�e�L�X�g�J�E���^ or GIF�_�~�[�摜�j
# sub_display_counter($n_counter, $flag_mode, $n_counter_digit)
sub sub_display_counter{
	# ����
	my $n_counter = shift;
	my $flag_mode = shift;
	my $n_counter_digit = shift;
	# �����`�F�b�N
	if(!defined($n_counter) || $n_counter eq '' || $n_counter<1){ $n_counter = 0; }
	if(!defined($flag_mode) || $flag_mode eq ''){ $flag_mode = 'null'; }
	if(!defined($n_counter_digit) || $n_counter_digit eq '' || $n_counter_digit<1){ $n_counter_digit = 4; }


	if ($flag_mode eq "text") {
		# �e�L�X�g�ŃJ�E���^�[��`��
		# �\�������ɍ��킹�āA���݂̃J�E���^�l�𕶎���ɂ���
		my $str = sprintf(sprintf("%%0%dld", $n_counter_digit), $n_counter);
		print("Content-type: text/html\n\n");
		print($str."\n");
		return;
	} elsif ($flag_mode eq "gif") {
		# GIF �̃_�~�[�摜��\������
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
# SQL�i�[�ɗL�Q�ɂȂ镶���������܂��̓G���R�[�h����
# sub_conv_to_safe_str(string, maxlength);
sub sub_conv_to_safe_str
{
	# ����
	my $str = shift;
	my $n_max_length = shift;
	# �����`�F�b�N
	if(!defined($str) || length($str)<=0){ $str = ''; }
	if(!defined($n_max_length) || $n_max_length<=0){ $n_max_length = 255; }

	# �C�ӂ̃G���R�[�h �� Shift JIS �ɕϊ�
	my $enc = guess_encoding($str, qw/euc-jp shiftjis 7bit-jis/);
	if(!ref($enc))
	{	# UTF8�͏�ɔ���G���[�ɂȂ邽��
		$str = encode('shiftjis', Encode::decode('utf8', $str));
	}
	else
	{
		$str = encode('shiftjis', decode($enc->name, $str));
	}

	# �o�C�i�������L�����폜�iSQL�C���W�F�N�V�����h�~�j
	# �i�Ȃ��A0x5c�̃o�b�N�X���b�V����SJIS�̕����R�[�h��N�H���邽�ߍ폜���Ȃ�
	$str =~ tr/(\x00-\x1f|\x21-\x27)//;
	$str =~ s/(\x2c|\x3b|<|>|\x3f|\x60)//g;


	# URL�G���R�[�h
#	$strTmp = uri_escape($strTmp);
	# �V���O���N�I�[�g�i'�j�����W���ł̓G���R�[�h����Ȃ����Ƃ̑Ώ�
#	$strTmp = uri_escape($strTmp, "'");

	# URL�G���R�[�h���ŏ��������s��		�i��̏����Œu���ς݂̂��߂��̏����s�v�j
#	$strTmp = uri_escape($strTmp, "%|");

	# ��������ő咷���Ő؂�̂Ă�
	$str = substr($str, 0, $n_max_length);
	
	return $str;
}

# *************
# �A�N�Z�X���O�p�̃f�[�^�𐶐�����
# array($ip,$hostname,$browser,$referer,$lang) = sub_make_accesslod_data()
sub sub_make_accesslod_data
{
	# �A�N�Z�X����IP�A�h���X
	my $ip = $ENV{'REMOTE_ADDR'};
	if(!defined($ip) || length($ip)<=0){ $ip = '0.0.0.0'; }
	
	$ip = sub_conv_to_safe_str($ip, 255);

	# �A�N�Z�X���̃z�X�g��
	my $hostname = $ENV{'REMOTE_HOST'};
	if(!defined($hostname) || length($hostname)<=0){ $hostname = ''; }
	if ($flag_ip_gethost && (($hostname eq "") || ($hostname eq $ip))) {
		if($ip eq '127.0.0.1' || $ip eq ''){ $hostname = 'localhost'; }
		elsif($ip eq '0.0.0.0'){ $hostname = ''; }
		else{ $hostname = gethostbyaddr(pack("C4", split(/\./, $ip)), 2); }
	}
	
	$hostname = sub_conv_to_safe_str($hostname, 255);

	# �u���E�U���i���[�U�G�[�W�F���g���j
	my $browser = $ENV{'HTTP_USER_AGENT'};
	if(!defined($browser) || length($browser)<=0){ $browser = ''; }

	$browser = sub_conv_to_safe_str($browser, 255);

	# �Ăяo���ꂽ���̃z�[���y�[�W���i���t�@���[�j
	my $referer = $ENV{'HTTP_REFERER'};
	if(!defined($referer) || length($referer)<=0){ $referer = ''; }
	if($referer eq "" && defined($ENV{'REQUEST_URI'}) &&  $ENV{'REQUEST_URI'} ne "") { $referer = $ENV{'REQUEST_URI'};}
	# �i�[�ł��Ȃ���������������
	$referer =~ s/%([0-9a-fA-F][0-9a-fA-F])/pack("C", hex($1))/eg;
	$referer =~ s/\\//g;
	$referer =~ s/\xa//eg;
	$referer =~ s/,//eg;

	# URL�̃h���C���������������āA�����񒷂�Z������
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


	# �C���X�g�[������
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
# �J�E���^�[DB�ǂݍ��݁i�J�E���^�[�l�A���b�N�|����A����IP�`�F�b�N�j
# sub_read_counter_db($ip, $flag_lock_mode)
sub sub_read_counter_db
{
	# ����
	my $ip = shift;	# IP�A�h���X
	my $flag_lock_mode = shift;	# ���b�N�@�\���g���ꍇ�A1
	# �����`�F�b�N
	if(!defined($ip) || $ip eq ''){ $ip = '0.0.0.0'; }
	if(!defined($flag_lock_mode) || $flag_lock_mode != 1 || $flag_lock_mode != 0){ $flag_lock_mode = 0; }

	# ���ݓ���
	my $tm_now = time();

	my $nLocked = 1;	# 1:���b�N�A0:����
	my $nCounter = 0;
	my $nSameIp = 0;	# 1:DB�ɓo�^����Ă���IP�A 0:DB�ɓo�^����Ă��Ȃ�

	my $tm_lock = 0;		# �ǂݏo�������b�N����
	
	my $dbh = undef;
	eval{
		# SQL�T�[�o�ɐڑ�
		$dbh = DBI->connect($str_dsn_counter, "", "", {PrintError => 0, AutoCommit => 1}) or die(DBI::errstr);

		# ���b�N�󋵂𓾂�
		my $str_query = "SELECT tm_epock FROM locktbl WHERE idx = '0'";
		my $sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);
		my @row = $sth->fetchrow_array();
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# �ė��p���邽��undef

		if(@row){ $tm_lock = $row[0]; }
		else{ $tm_lock = 0; }		# �l���Ȃ��ꍇ�� UNIX���� 0 �Ƃ���
		@row = ();	# �ė��p���邽�ߔz�񏉊���

		if($tm_now - $tm_lock > $tm_lock_timeout){ $nLocked = 0; }
		else{ $nLocked = 1;}

		# ���݂̃J�E���^�[�l�𓾂�
		$str_query = "SELECT counter FROM countertbl WHERE idx = '0'";
		$sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);
		@row = $sth->fetchrow_array();
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# �ė��p���邽��undef
		
		if(@row){ $nCounter = $row[0]; }
		else{ $nCounter = 0; }
		@row = ();	# �ė��p���邽�ߔz�񏉊���

		# �w�肳�ꂽIP�A�h���X�����ɓo�^����Ă��邩�`�F�b�N����
		$str_query = "SELECT COUNT (*) FROM iptbl WHERE ip = ?";
		$sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute($ip) or die(DBI::errstr);
		@row = $sth->fetchrow_array();
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# �ė��p���邽��undef
		
		if(@row && $row[0] > 0){ $nSameIp = 1; }
		else{ $nSameIp = 0; }

		if($flag_lock_mode == 1 && $nLocked == 0)	# ���b�N���|�����Ă��Ȃ��ꍇ
		{
			# ����2�̏����̂ǂ��炩�̎��Ƀ��O���L�^����
			# 1. ���b�N����Ă��炸�A$flag_ip_sameaddr_check==0�̂Ƃ�
			# 2. ���b�N����Ă��炸�A$flag_ip_sameaddr_check==1����$nSameIp==0�̂Ƃ�
			if($flag_ip_sameaddr_check == 0 || $nSameIp == 0)
			{
				$str_query = "UPDATE locktbl SET tm_epock = ? WHERE idx = '0'";
				$sth = $dbh->prepare($str_query) or die(DBI::errstr);
				$sth->execute($tm_now) or die(DBI::errstr);
				
				$tm_lock = $tm_now;
			}
		}
		

		# DB�����
		$dbh->disconnect() or die(DBI::errstr);
	};
	if($@){
		# eval�ɂ��DB�G���[�g���b�v�F�G���[���̏���
		if(defined($dbh)){ $dbh->disconnect(); }
		my $str = $@;
		chomp($str);
		sub_write_to_debug_log("===SQLERROR_COUNTERDB_READ (".$str.")===");
	}
	
	return($nLocked, $nCounter, $nSameIp, $tm_lock);
}

# *************
# �J�E���^�[DB�A�b�v�f�[�g�i�J�E���^�[�������݁A���b�N�����AIP�������݁j
# sub_write_counter_db($ip, $n_counter)
sub sub_write_counter_db
{
	# ����
	my $ip = shift;
	my $n_counter = shift;
	# �����`�F�b�N
	if(!defined($ip) || $ip eq ''){ $ip = '0.0.0.0'; }
	if(!defined($n_counter) || $n_counter eq '' || $n_counter <= 0){ $n_counter = 0; }

	# ���ݓ���
	my $tm_now = time();

	my $n_index = 0;	# �A�N�Z�X��ip�A�h���X���X�g�̃C���f�b�N�X

	my $dbh = undef;
	eval{
		# SQL�T�[�o�ɐڑ�
		$dbh = DBI->connect($str_dsn_counter, "", "", {PrintError => 0, AutoCommit => 1}) or die(DBI::errstr);

		# ���݂̃J�E���^�[�l����������
		my $str_query = "UPDATE countertbl SET counter = ? WHERE idx = '0'";
		my $sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute($n_counter) or die(DBI::errstr);
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# �ė��p���邽��undef

		# �ł��Â����Ԃ́u�w��IP�A�h���X�v�s�̃C���f�b�N�X�����o��
#			$str_query = "SELECT idx FROM iptbl ORDER BY tm_epock LIMIT 1";
		$str_query = "SELECT idx FROM iptbl WHERE ip = ? ORDER BY tm_epock LIMIT 1";
		$sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute($ip) or die(DBI::errstr);
		my @row = $sth->fetchrow_array();
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# �ė��p���邽��undef
		
		if(@row)
		{	# IP�A�h���X�����ɓo�^����Ă����ꍇ
			$n_index = $row[0];
		}
		else
		{
			@row = ();	# �ė��p���邽�ߔz�񏉊���
			# �ł��Â����Ԃ̃C���f�b�N�X�����o��
			$str_query = "SELECT idx FROM iptbl ORDER BY tm_epock LIMIT 1";
			$sth = $dbh->prepare($str_query) or die(DBI::errstr);
			$sth->execute() or die(DBI::errstr);
			@row = $sth->fetchrow_array();
			$sth->finish() or die(DBI::errstr);
			$sth = undef;	# �ė��p���邽��undef
			
			if(@row) { $n_index = $row[0]; }
			else { $n_index = 0; }
		}

		# IP�A�h���X�A���ݎ�������������
		$str_query = "UPDATE iptbl SET ip = ?, tm_epock = ? WHERE idx = ?";

		$sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute($ip, $tm_now, $n_index) or die(DBI::errstr);
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# �ė��p���邽��undef

		# ���b�N����
		$str_query = "UPDATE locktbl SET tm_epock = '0' WHERE idx = '0'";
		$sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);
		$sth->finish() or die(DBI::errstr);
		

		# DB�����
		$dbh->disconnect() or die(DBI::errstr);
	};
	if($@){
		# eval�ɂ��DB�G���[�g���b�v�F�G���[���̏���
		if(defined($dbh)){ $dbh->disconnect(); }
		my $str = $@;
		chomp($str);
		sub_write_to_debug_log("===SQLERROR_COUNTERDB_UPDATE (".$str.")===");
	}
	
	return;
}


# *************
# �d������pIP�A�h���X���X�g��\���p�ɓǂݏo���i�e�X�g���[�h�p�j
sub sub_textout_iplist
{
	my $str = "<p>info : IP address list in counter DB<br />\n";
	
	my $dbh = undef;
	eval{
		# SQL�T�[�o�ɐڑ�
		$dbh = DBI->connect($str_dsn_counter, "", "", {PrintError => 0, AutoCommit => 1}) or die(DBI::errstr);

		# IP�A�h���X���X�g��S�ēǂݏo��
		my $str_query = "SELECT * FROM iptbl ORDER BY tm_epock";
		my $sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);

		my @row;
		while(@row = $sth->fetchrow_array())
		{
			$str .= sprintf("&nbsp;&nbsp;idx=%02d, ip=%-15s, time=%d/%02d/%02d %02d:%02d:%02d<br />\n", $row[0], $row[1], (localtime($row[2]))[5]+1900, (localtime($row[2]))[4]+1, (localtime($row[2]))[3], (localtime($row[2]))[2], (localtime($row[2]))[1], (localtime($row[2]))[0]);
		}

		$sth->finish() or die(DBI::errstr);
		# DB�����
		$dbh->disconnect() or die(DBI::errstr);
	};
	if($@){
		# eval�ɂ��DB�G���[�g���b�v�F�G���[���̏���
		if(defined($dbh)){ $dbh->disconnect(); }
		my $str = $@;
		chomp($str);
		sub_write_to_debug_log("===SQLERROR_COUNTERDB_READ (".$str.")===");
	}

	$str .= "</p>\n";
	
	return($str);
}

# *************
# �A�N�Z�X���O�f�[�^�x�[�X�ɁA���O���P�s��������
# sub_write_accesslog_db($ip,$hostname,$browser,$referer,$lang)
sub sub_write_accesslog_db
{
	# ����
	my $ip = shift;
	my $hostname = shift;
	my $browser = shift;
	my $referer = shift;
	my $lang = shift;
	# �����`�G�b�N
	if(!defined($ip) || $ip eq ''){ $ip = ''; }
	if(!defined($hostname) || $hostname eq ''){ $hostname = ''; }
	if(!defined($browser) || $browser eq ''){ $browser = ''; }
	if(!defined($referer) || $referer eq ''){ $referer = ''; }
	if(!defined($lang) || $lang eq ''){ $lang = ''; }

	# ���ݓ���
	my $tm_now = time();

	# ����
	my ($year,$month,$day,$hour,$min,$sec,$n_weekday,$n_day_second) = sub_make_datetime_str();

	my $dbh = undef;
	eval{
		# DB�ɐڑ�
		$dbh = DBI->connect($str_dsn_accesslog, "", "", {PrintError => 0, AutoCommit => 1}) or die(DBI::errstr);
		# �N�G���쐬
		my $str_query = "insert into acctbl values(null,?,?,?,?,?,?,?,?)";
		my $sth = $dbh->prepare($str_query) or die(DBI::errstr);

		# �N�G�����ߔ��s
		$sth->execute($tm_now, $n_weekday, $n_day_second, $ip, $hostname, $browser, $referer, $lang) or die(DBI::errstr);
		$sth->finish() or die(DBI::errstr);

		# DB�����
		$dbh->disconnect() or die(DBI::errstr);
		
	};
	if($@){
		# eval�ɂ��DB�G���[�g���b�v�F�G���[���̏���
		if(defined($dbh)){ $dbh->disconnect(); }
		my $str = $@;
		chomp($str);
		sub_write_to_debug_log("===SQLERROR_ACCDB (".$str.")===");
	}

	return;
}


# *************
# �e�X�g���[�h�i���\���j
# sub_test_cgi($ip,$hostname,$browser,$referer,$lang, $tm_lock, $flag_lock, $flag_sameip, $n_counter);
sub sub_test_cgi
{
	# ����
	my $ip = shift;
	my $hostname = shift;
	my $browser = shift;
	my $referer = shift;
	my $lang = shift;
	my $tm_lock = shift;
	my $flag_lock = shift;
	my $flag_sameip = shift;
	my $n_counter = shift;
	
	# ���ݓ���
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
		# SQL�T�[�o�ɐڑ�
		$dbh = DBI->connect($str_dsn_accesslog, "", "", {PrintError => 1, AutoCommit => 0}) or die(DBI::errstr);

		# �A�N�Z�X�f�[�^�x�[�X�S�̂̃f�[�^��
		my $str_query = "SELECT COUNT (*) FROM acctbl";
		my $sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);
		my @row = $sth->fetchrow_array();
		$sth->finish() or die(DBI::errstr);

		$str .= "<p>info : Access Log DB ".$row[0]." lines exist, OK</p>\n";
		$dbh->disconnect() or die(DBI::errstr);

	};
	if($@){
		# eval�ɂ��DB�G���[�g���b�v�F�G���[���̏���
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
# �G���[���̃f�o�b�O���OCSV�t�@�C���ւ̏�������
# sub_write_to_debug_log($str)
sub sub_write_to_debug_log
{
	my $str = shift;

	# ���ݓ���
	my $tm_now = time();

	# �~���b�P�ʂł̏����J�n����̌o�ߎ���
	my $str_millitime_lapse = Time::HiRes::time() - $tmmStart;
	# ���ݓ���
	my $str_time_now .= sprintf("%d/%02d/%02d,%02d:%02d:%02d", (localtime($tm_now))[5]+1900, (localtime($tm_now))[4]+1, (localtime($tm_now))[3], (localtime($tm_now))[2], (localtime($tm_now))[1], (localtime($tm_now))[0]);
	# ���b�N����
	my $str_time_lock .= sprintf("%d/%02d/%02d,%02d:%02d:%02d", (localtime($tm_lock))[5]+1900, (localtime($tm_lock))[4]+1, (localtime($tm_lock))[3], (localtime($tm_lock))[2], (localtime($tm_lock))[1], (localtime($tm_lock))[0]);

	# �f�o�b�O�p�t�@�C���ɏ�������
	open(OUT, ">> $str_filepath_debugcsv");	# �ǋL���[�h
	
	print(OUT $str_time_now.",lock=".$str_time_lock.",laps=".$str_millitime_lapse.",argv=".(defined($ARGV[0])?$ARGV[0]:'').",ip=".$ip.", hostname=".$hostname.",browser=".$browser.",referer=".$referer.",lock=".$flag_is_locked.",sameip=".$flag_is_ip_sameaddr);
	if(defined($str) && $str ne '') { print(OUT ",".$str);}
	print(OUT "\n");
	close(OUT);

}

# �����f�[�^�̕�������쐬����
# ($year,$month,$day,$hour,$min,$sec,$n_weekday,$n_day_second) = 
#   sub_make_datetime_str()
sub sub_make_datetime_str{

	# ���ݓ���
	my $tm_now = time();
	# �j�������߂�
	my $n_weekday = (localtime($tm_now))[6];
	my ($sec, $min, $hour, $day, $month, $year) = localtime($tm_now);
	# ���̓���0������̕b��
	my $n_day_second = $hour*60*60 + $min*60 + $sec;

	return($year,$month,$day,$hour,$min,$sec,$n_weekday,$n_day_second);

}

# ************* (EOF)
