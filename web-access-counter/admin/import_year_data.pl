#!/usr/bin/perl

# save this file in << Shift JIS  >> encode !

use strict;

# ���t�I�u�W�F�N�g��p����
use Time::Local;
# �f�[�^�x�[�X�I�u�W�F�N�g��p����
use DBI;

use URI::Escape;
use File::Basename;
use Encode;
use Encode::Guess;
use FindBin qw/$Bin/;	# �T�[�o��ł̃t���p�X���𓾂邽��

# �~���b�𗘗p����
use Time::HiRes;

# ����CSV�t�@�C�������肷��ϐ�
my $nTargetYear = 0;
my $strCsvFilename = $FindBin::Bin.'/../backup/';

# �f�[�^�x�[�X �n���h��
my $dbh = undef;
my $sth = undef;

# �ϐ�
my $nTotalData = 0;		# �f�[�^�x�[�X�S�̂ł̑S�f�[�^��


# ��Ɨp�ϐ�
my $strQuery = undef;	# �N�G�����s�����߂̈ꎞ�I�ȃN�G�����ߕ�
my @row = ();		# �N�G�����ʂ��󂯂�z��

my $strTmp = "";
my @arrayTmp;		# CSV�t�@�C���̍s�f�[�^��؂蕪�����z��
my $tmTmp = timelocal(0,0,0,1,0,0);
my $tm2000 = timelocal(0,0,0,1,0,2000-1900);	# 2000/1/1 00:00
my $tm2032 = timelocal(0,0,0,1,0,2032-1900);	# 2032/1/1 00:00
my $nWday = 0;
my $nDaySeconds = 0;

my $i = 0;
my $j = 0;


my $tmQueryTime = undef;		# �N�G���ɂ�����������
my $tmQueryStartTime = undef;	# �N�G���J�n����
my $tmQueryEndTime = undef;		# �N�G���I������

# SQL�T�[�o��DSN��`
my $strDbPath = $FindBin::Bin.'/../data/';
my $strSqlDsn = 'DBI:SQLite:dbname='.$FindBin::Bin.'/../data/';
my $strDefaultDB = 'accdb.backup.sqlite';

print("\n=== Import access data from CSV file ===\n");


# �����i�Ώ۔N�j�̏����B�܂��̓R���\�[������Ώ۔N�ADB����ǂݍ���
if($#ARGV != 0) {
	# �v���O�����ւ̈������Ȃ��ꍇ


	$strDefaultDB = sub_select_dbfile($strDbPath);	# �Ώ� DB �t�@�C���̑I��
	$strSqlDsn .= $strDefaultDB;
	
	# �Ώ۔N�̑Θb�I����
	print("input target year (from 2000 to 2032) : ");
	$_ = <STDIN>;
	chomp();		# �s���̉��s�����
	$nTargetYear = int($_);		# ���������o��

}
else
{
	# ����������΁A�����Ώ۔N�Ƃ��ēǂݍ���
	$nTargetYear = sprintf("%d", $ARGV[0]);
	
	# �W����DB����p����
	$strSqlDsn .= $strDefaultDB;
}

# �^�[�Q�b�g�N�A�o�̓t�@�C�������m��
$strCsvFilename = $strCsvFilename . "backup". $nTargetYear . ".csv";
print("target DB :". $strSqlDsn . "\n");
print("target year :". $nTargetYear . "\n");
print("import from csv :". $strCsvFilename . "\n");

if($nTargetYear < 2000 || $nTargetYear > 2032)
{
	print("error : target year must be within 2000 and 2032\n");
	exit();
}

# CSV�t�@�C���̑��݂��m�F
if(!(-r $strCsvFilename))
{
	print("error: CSV file is not exist or not readable\n");
	exit();
}

print("\nimportant ! : sure to IMPORT ? (y/n): ");
$_ = <STDIN>;
chomp();	# �s���̉��s�����
if(uc($_) ne "Y")
{
	print("info : cancel\n");
	exit();
}


# �N�G���ɂ����鎞�Ԃ��v�����邽�߁A�J�n������ۑ�
$tmQueryStartTime = Time::HiRes::time();

# SQL�T�[�o�ɐڑ�
$dbh = DBI->connect($strSqlDsn, "", "", {PrintError => 1, AutoCommit => 0}) or die("error : database open error\n");

eval{

	if(open(IN, "< $strCsvFilename"))
	{

		$i = 0;		# �f�[�^���J�E���^

		while(<IN>) {	#********* WHILE LOOP START HERE *********
			$strTmp = $_;
			if(length($strTmp) < 10){ next;}


	#	CSV�͎��̂悤�ɔz��ɓ��͂����
	#	$count,$year,$mon,$mday,$hour,$min,$sec,$proxy_name,$remip,$userip,$proxy_str,$browser,$referer,$lang,$last_acc_log

			@arrayTmp = split(/,/, $_);

			#�z��v�f��8�����iIP�A�h���X���ڂ������j�̂Ƃ��́A���̍s���X�L�b�v
			if($#arrayTmp < 8) { next;}

			# ������UNIX�����ɕϊ�
			$tmTmp = timelocal($arrayTmp[6], $arrayTmp[5], $arrayTmp[4], $arrayTmp[3], $arrayTmp[2]-1, $arrayTmp[1]-1900);

			#���������������ꍇ�i2000�N�ȑO�A2032�N�ȍ~�j�A���̍s���X�L�b�v
			if($tmTmp < $tm2000 || $tmTmp > $tm2032) { next;}

			# �j�������߂�
			$nWday = (localtime($tmTmp))[6];
		
			# ���̓���0������̕b��
			$nDaySeconds = $arrayTmp[4]*60*60 + $arrayTmp[5]*60 + $arrayTmp[6];

			# ���ɓo�^����Ă���f�[�^�łȂ���1000�s�����Ƀ`�F�b�N
			if($i % 1000 == 0)
			{
				printf("info : processing ".$i." data ...\n");	# �o�߃��b�Z�[�W�\��

				$strQuery = "select count (*) from acctbl where tm_epock == '".$tmTmp."' and ip == '".$arrayTmp[8]."'";
				$sth = $dbh->prepare($strQuery);
				if($sth){ $sth->execute();}
				my @row = $sth->fetchrow_array();
				if($row[0] != 0)
				{
					$dbh->rollback;
					$dbh->disconnect();
					print("error : same data exist\n");
					exit();
				}
				if($sth){ $sth->finish();}
			}

			# SQL�����\�z����
			$strQuery = "insert into acctbl values(null,".$tmTmp.",".$nWday.",".$nDaySeconds.",'".$arrayTmp[8]."','".&SanitizeString($arrayTmp[7],255)."','".&SanitizeString($arrayTmp[11],255)."','".&SanitizeString($arrayTmp[12],255)."','".$arrayTmp[13]."')";
			
			##### DEBUG
	#		print("info: ".$strSQL."\n");
			##### DEBUG

			$sth = $dbh->prepare($strQuery);
			if($sth){ $sth->execute();}
			if($sth){ $sth->finish();}
			$i++;			# �f�[�^���J�E���^

		}
		
		# �������݂̃R�~�b�g
		print("info : DB commit\n");
		$dbh->commit;

		# CSV�t�@�C�������
		close(IN);
	}

	# SQL�ؒf
	$dbh->disconnect();
	print("info : DB close nomally\n");

};
if($@){
# eval�ɂ��G���[�g���b�v�F�G���[���̏���
	$dbh->rollback;
	$dbh->disconnect();
	print("\nError : ".$@."\n");
	exit();
}

# �N�G���ɂ����鎞�Ԃ��v�����邽�߁A�I��������ۑ�
$tmQueryEndTime = Time::HiRes::time();
# �N�G���ɂ�����������
$tmQueryTime = $tmQueryEndTime - $tmQueryStartTime;


print("info : ".$i." lines added, process time is ".$tmQueryTime." sec\n");

# ���f�[�^���𓾂�i�\���p�j
eval{
	# SQL�T�[�o�ɐڑ�
	$dbh = DBI->connect($strSqlDsn, "", "", {PrintError => 1, AutoCommit => 0});
	if(!$dbh){
		print("error : database open error\n");
		exit;
	}

	$sth = $dbh->prepare("SELECT COUNT (*) FROM acctbl");
	if($sth){ $sth->execute();}
	my @row = $sth->fetchrow_array();
	if(@row)
	{
		print("info : total data is ".$row[0]." lines\n");
	}
	if($sth){ $sth->finish();}

	$dbh->disconnect();
	print("info : DB close nomally\n");

};
if($@){
# eval�ɂ��G���[�g���b�v�F�G���[���̏���
	$dbh->disconnect();
	print("Error : ".$@."\n");
	exit();
}



exit();


# SQL�i�[�ɗL�Q�ɂȂ镶���������܂��̓G���R�[�h����
# SanitizeString(char *string, int maxlength);
sub SanitizeString
{
	# ���[�J���ϐ��Ɉ�������
	my($strTmp) = $_[0];

	my $enc = guess_encoding($strTmp, qw/euc-jp shiftjis 7bit-jis/);
	if(!ref($enc))
	{	# UTF8�͏�ɔ���G���[�ɂȂ邽��
		$strTmp = encode('shiftjis', Encode::decode('utf8', $strTmp));
	}
	else
	{
		$strTmp = encode('shiftjis', decode($enc->name, $strTmp));
	}

	$strTmp =~ s/,//eg;				# �R���}����
	$strTmp =~ s/\x27//eg;			# �V���O���N�I�[�g�i�f�j����
	$strTmp =~ s/\x22//eg;			# �_�u���N�I�[�g�i�h�j����
	$strTmp =~ tr/\x0-\x1f//d;		# 0x00�`0x1f�̃o�C�i������
	
	# �����񖖒[��CR/LF������ꍇ�͏�������
#	chomp($strTmp);

	# URL�G���R�[�h
#	$strTmp = uri_escape($strTmp);
	# �V���O���N�I�[�g�i'�j�����W���ł̓G���R�[�h����Ȃ����Ƃ̑Ώ�
#	$strTmp = uri_escape($strTmp, "'");

	# URL�G���R�[�h���ŏ��������s��
	$strTmp = uri_escape($strTmp, "%|");

	# ��������ő咷���Ő؂�̂Ă�
	$strTmp = substr($strTmp, 0, $_[1]);
	
	return $strTmp;
}


# SQLite �f�[�^�x�[�X�t�@�C���ꗗ���A�Ώۃt�@�C����I������
sub sub_select_dbfile {
	my $strSearchPath = shift;
	$strSearchPath .= '*.sqlite';

	my @arrScan = glob($strSearchPath);

	if($#arrScan < 0){ die("Cannot find SQLite DB data file, at $strSearchPath \n"); }

	print("database file list\n");
	my $i=0;
	foreach(@arrScan){
		print(" ".++$i." : ".basename($_)."\n");
	}
	print("select target database from 1 to ".$i.". (1-".$i.") : ");
	$_ = <STDIN>;
	chomp();
	if(int($_)<1 || int($_)>$i){ die("input is out of range\n"); }
	
	my $strSelectedFile = basename($arrScan[int($_)-1]);

	print("user select : ".$strSelectedFile."\n");
	
	return($strSelectedFile);

}

