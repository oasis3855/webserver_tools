#!/usr/bin/perl

# save this file in << Shift JIS  >> encode !

use strict;

# ���t�I�u�W�F�N�g��p����
use Time::Local;
# �f�[�^�x�[�X�I�u�W�F�N�g��p����
use DBI;

use URI::Escape;
use Encode;
use Encode::Guess;
use File::Basename;
use FindBin qw/$Bin/;	# �T�[�o��ł̃t���p�X���𓾂邽��

# �~���b�𗘗p����
use Time::HiRes;

# �o��CSV�t�@�C�������肷��ϐ�
my $nTargetYear = 0;
my $strCsvFilename = $FindBin::Bin.'/../backup/';

# �f�[�^�x�[�X �n���h��
my $dbh = undef;
my $sth = undef;

# �ϐ�
my $nTotalData = 0;		# �f�[�^�x�[�X�S�̂ł̑S�f�[�^��

# �ꎞ�ϐ�
my $strQuery = undef;	# �N�G�����s�����߂̈ꎞ�I�ȃN�G�����ߕ�

my $strTmp = "";
my $nStartEpockSec = 0;
my $nEndEpockSec = 0;
my @arrWdayStr = ("Sun","Mon","Tue","Wed","Thu","Fri","Sat");

my $i = 0;
my $j = 0;

my @row = ();		# �N�G�����ʂ��󂯂�z��

# SQL�T�[�o��DSN��`�iDB���͑Θb�I�ɓ��͏������s���j
my $strDbPath = $FindBin::Bin.'/../data/';
my $strSqlDsn = 'DBI:SQLite:dbname='.$FindBin::Bin.'/../data/';
my $strDefaultDB = 'accdb.sqlite';

# �������b�Z�[�W�̕\��
print("\n=== Delete access data from DB ===\n");

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

print("target DB :". $strSqlDsn . "\n");
print("target year :". $nTargetYear . "\n");

if($nTargetYear < 2000 || $nTargetYear > 2032)
{
	print("error : target year must be within 2000 and 2032\n");
	exit();
}

print("\nimportant ! : sure to DELETE ? (y/n): ");
$_ = <STDIN>;
chomp();	# �s���̉��s�����
if(uc($_) ne "Y")
{
	print("info : cancel\n");
	exit();
}


print("info : deleting ...\n");

# UNIX���ԂŊJ�n�E�I���b�����
$nStartEpockSec = timelocal(0,0,0,1,0,$nTargetYear-1900);
$nEndEpockSec = timelocal(59,59,23,31,11,$nTargetYear-1900);

print("target time span : from ".$nStartEpockSec." to ".$nEndEpockSec."\n");

# SQL�T�[�o�ɐڑ�
$dbh = DBI->connect($strSqlDsn, "", "", {PrintError => 1, AutoCommit => 0}) or die("error : database open error\n");


eval{

	# �f�[�^�x�[�X�S�̂̃f�[�^��
	$strQuery = "DELETE FROM acctbl WHERE tm_epock >= '".$nStartEpockSec."' AND tm_epock <= '".$nEndEpockSec."'";
	$sth = $dbh->prepare($strQuery);
	if($sth){ $sth->execute();}
	@row = $sth->fetchrow_array();
	$nTotalData = $row[0];

	if($sth){ $sth->finish();}

	print("info : ".$nTotalData." lines is deleted\n");

	# �������݂̃R�~�b�g
	$dbh->commit;
	print("info : DB commit\n");

	# DB�̍č\�z�i�t���[�X�y�[�X���l�߂�j
	$dbh->{AutoCommit} = 1;
	$sth = $dbh->do("VACUUM acctbl");

	print("info : vacuum complete\n");

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


exit();


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


