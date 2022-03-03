<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="ja" lang="ja">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<meta http-equiv="Content-Language" content="ja" />
<title>access log query</title>
</head>

<body>
	<p>アクセスログ抽出クエリ</p>

<?php
// save this file in << UTF-8  >> encode !
// ******************************************************
// Software name : アクセスログ抽出クエリ for Webページ用 カウンタCGI
//
// Copyright (C) INOUE Hirokazu, All Rights Reserved
//     http://oasis.halfmoon.jp/
//
// query.php
//
// version 3.0, 2011/03/26  utf8化
//
// GNU GPL Free Software
//
// このプログラムはフリーソフトウェアです。あなたはこれを、フリーソフトウェア財
// 団によって発行された GNU 一般公衆利用許諾契約書(バージョン2か、希望によっては
// それ以降のバージョンのうちどれか)の定める条件の下で再頒布または改変することが
// できます。
//
// このプログラムは有用であることを願って頒布されますが、*全くの無保証* です。
// 商業可能性の保証や特定の目的への適合性は、言外に示されたものも含め全く存在し
// ません。詳しくはGNU 一般公衆利用許諾契約書をご覧ください。
//
// あなたはこのプログラムと共に、GNU 一般公衆利用許諾契約書の複製物を一部受け取
// ったはずです。もし受け取っていなければ、フリーソフトウェア財団まで請求してく
// ださい(宛先は the Free Software Foundation, Inc., 59 Temple Place, Suite 330
// , Boston, MA 02111-1307 USA)。
//
// http://www.opensource.jp/gpl/gpl.ja.html
// ******************************************************

$strThisScriptName = basename(__FILE__);

// SQLサーバのDSN定義
$strSqlDsn = 'sqlite:'.dirname(__FILE__).'/../data/';

$strDefaultDB = "accdb.sqlite";
if(!empty($_GET['db']) && $_GET['db'] === 'backup'){ $strDefaultDB = "accdb.backup.sqlite"; }

// DSNを完成させる
$strSqlDsn = $strSqlDsn . $strDefaultDB;
print("<p>利用データベース : ".$strDefaultDB."</p>\n");

$flStartTime = 0.0;		// クエリ開始時間
$flEndTime = 0.0;		// クエリ終了時間

$nStartEpockSec = 0;	// 検索範囲（最古）日時
$nEndEpockSec = 0;		// 検索範囲（最新）日時

$nQuery = 0;		// クエリ件数
$nTotalData = 0;	// データベース中の全データ数

if(!empty($_POST['query_command']) && $_POST['query_command'] == 'start')
{

	// 開始日、終了日の文字列が数値だけか判定
	if(!is_numeric($_POST['start_year']) || !is_numeric($_POST['start_month'])
		|| !is_numeric($_POST['start_day']) || !is_numeric($_POST['end_year'])
		|| !is_numeric($_POST['end_month']) || !is_numeric($_POST['end_day'])
		|| !is_numeric($_POST['start_hour']) || !is_numeric($_POST['end_hour'])
		|| !is_numeric($_POST['start_min']) || !is_numeric($_POST['end_min'])
		)
	{
		ErrorDispAndExit("開始日、終了日に数値以外の文字が入力されています");
	}

	// 開始日を、UNIXエポック秒に変換
	$nStartEpockSec = mktime($_POST['start_hour'],$_POST['start_min'],0,$_POST['start_month'],$_POST['start_day'],$_POST['start_year']);
	// 終了日を、UNIXエポック秒に変換
	$nEndEpockSec = mktime($_POST['end_hour'],$_POST['end_min'],59,$_POST['end_month'],$_POST['end_day'],$_POST['end_year']);
	
	$strTemp = sprintf("日付範囲 %04d/%02d/%02d %02d/%02d（%d） 〜 %04d/%02d/%02d %02d:%02d (%d)",
		$_POST['start_year'], $_POST['start_month'], $_POST['start_day'],$_POST['start_hour'],$_POST['start_min'],
		$nStartEpockSec,
		$_POST['end_year'], $_POST['end_month'], $_POST['end_day'],$_POST['end_hour'],$_POST['end_min'],
		$nEndEpockSec);
	
	print("<p>&nbsp;</p>\n<p>$strTemp</p>\n");

	
	$strQueryCore = "FROM acctbl WHERE tm_epock >= $nStartEpockSec AND tm_epock <= $nEndEpockSec";
	
	if(!empty($_POST['ip']))
	{
		$strQueryCore .= " AND ip LIKE '".EscapeSqlString(mb_convert_encoding($_POST['ip'],'SJIS','UTF8'))."'";
	}
	if(!empty($_POST['hostname']))
	{
		$strQueryCore .= " AND hostname LIKE '".EscapeSqlString(mb_convert_encoding($_POST['hostname'],'SJIS','UTF8'))."'";
	}
	if(!empty($_POST['useragent']))
	{
		$strQueryCore .= " AND browser LIKE '%".EscapeSqlString(mb_convert_encoding($_POST['useragent'],'SJIS','UTF8'))."%'";
	}
	if(!empty($_POST['referer']))
	{
		$strQueryCore .= " AND referer LIKE '%".EscapeSqlString(mb_convert_encoding($_POST['referer'],'SJIS','UTF8'))."%'";
	}
	if(!empty($_POST['language']))
	{
		$strQueryCore .= " AND lang LIKE '%".EscapeSqlString(mb_convert_encoding($_POST['language'],'SJIS','UTF8'))."%'";
	}
	
	if(!empty($_POST['sort']))
	{
		if($_POST['sort'] == "day_asc") $strQueryCore .= " ORDER BY tm_epock LIMIT 100";
		if($_POST['sort'] == "day_desc") $strQueryCore .= " ORDER BY tm_epock DESC LIMIT 100";
		if($_POST['sort'] == "ip") $strQueryCore .= " ORDER BY ip LIMIT 100";
	}
	
	print("<p>SQL : SELECT * $strQueryCore</p>\n");

	try {

		// データベースを開く
		$dbh = new PDO($strSqlDsn);
		// エラー時の例外処理を有効とする
		$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		// クエリ発行（データベースの全データ数確認）
		$strQuery = "SELECT COUNT (*) FROM acctbl";
		$sth = $dbh->query($strQuery);
		$rs = $sth->fetch();
		$nTotalData = $rs[0];

		// クエリ発行（クエリ結果総数確認）
		$strQuery = "SELECT COUNT (*) " . $strQueryCore;
		$sth = $dbh->query($strQuery);
		$rs = $sth->fetch();
		if($rs[0] > 100)
		{
			print("<p><span style=\"color:red;\">総数 : ".$rs[0]."（100件を超えていますので、最初の１００件を表示します）</span></p>\n");
		}
		else
		{
			print("<p>総数 : ".$rs[0]."</p>\n");
		}

		// クエリ発行
		$strQuery = "SELECT * " . $strQueryCore;
		$flStartTime = microtime(true);
		$sth = $dbh->query($strQuery);
		$flEndTime = microtime(true);
		
		// 画面表示（テーブルヘッダ）
		print("\n<table style=\"border-color: rgb(0,0,0); border-collapse: collapse; font-size: 12px;\" border=\"1\" cellpadding=\"0\" cellspacing=\"1\">\n" .
		"<tr><td>日時</td><td>IPアドレス</td><td>ホスト名</td><td>ブラウザ文字列</td><td>閲覧ページ</td><td>言語</td></tr>\n");

		// クエリ結果よりデータを1行ずつ取り出す
		while ($rs = $sth->fetch())
		{
			// SQLite dbはSJISで格納されているため、utf-8に変換
			if(!empty($rs['browser'])){ $rs['browser'] = mb_convert_encoding($rs['browser'],'utf-8','SJIS'); }
			if(!empty($rs['referer'])){ $rs['referer'] = mb_convert_encoding($rs['referer'],'utf-8','SJIS'); }
			// 画面表示（1行ずつデータを表示する）
			print("<tr><td>".date("Y/m/d(D) H:i:s", $rs['tm_epock'])."</td><td>".$rs['ip']."</td><td>".htmlspecialchars($rs['hostname'])."</td><td>".htmlspecialchars($rs['browser'])."</td><td>".htmlspecialchars($rs['referer'])."</td><td>".$rs['lang']."</td></tr>\n");
			$nQuery++;
		}
		print("</table>\n");
		
		// データベースを明示的に閉じる
		$dbh = null;
	
	} catch(PDOException $exception) {
		// SQLアクセスでエラーが発生した時の処理
		$strMsg = "SQL : ".$exception->getMessage();
		print("Error: ".$strMsg);
//		ErrorDispAndExit($strMsg);
	}


}
else
{
// phpスクリプトに引数がない場合、入力画面を表示する

	try {

		// データベースを開く
		$dbh = new PDO($strSqlDsn);
		// エラー時の例外処理を有効とする
		$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$flStartTime = microtime(true);

		// クエリ発行（データベースの全データ数確認）
		$strQuery = "SELECT COUNT (*) FROM acctbl";
		$sth = $dbh->query($strQuery);
		$rs = $sth->fetch();
		$nTotalData = $rs[0];

		// クエリ発行（最も古い日付を得る）
		$strQuery = "SELECT MIN(tm_epock) FROM acctbl";
		$sth = $dbh->query($strQuery);
		$rs = $sth->fetch();
		$nStartEpockSec = $rs[0];

		// クエリ発行（最も新しい日付を得る）
		$strQuery = "SELECT MAX(tm_epock) FROM acctbl";
		$sth = $dbh->query($strQuery);
		$rs = $sth->fetch();
		$nEndEpockSec = $rs[0];

		$flEndTime = microtime(true);

		// データベースを明示的に閉じる
		$dbh = null;


	} catch(PDOException $exception) {
		// SQLアクセスでエラーが発生した時の処理
		$strMsg = "SQL : ".$exception->getMessage();
		print("Error: ".$strMsg);
//		ErrorDispAndExit($strMsg);
	}

?>
<form method="post" action="<?php echo $strThisScriptName; echo ($strDefaultDB === "accdb.backup.sqlite"?'?db=backup':''); ?>">
	<p>期間指定 ： <input type="text" name="start_year" size="5" value="<?php echo date("Y", $nStartEpockSec);?>" />年<input type="text" name="start_month" size="5" value="<?php echo date("m", $nStartEpockSec);?>" />月<input type="text" name="start_day" size="5" value="<?php echo date("d", $nStartEpockSec);?>" />日 <input type="text" name="start_hour" size="5" value="<?php echo date("H", $nStartEpockSec);?>" />時<input type="text" name="start_min" size="5" value="<?php echo date("i", $nStartEpockSec);?>" />分
	〜 <input type="text" name="end_year" size="5" value="<?php echo date("Y", $nEndEpockSec);?>" />年<input type="text" name="end_month" size="5" value="<?php echo date("m", $nEndEpockSec);?>" />月<input type="text" name="end_day" size="5" value="<?php echo date("d", $nEndEpockSec);?>" />日 <input type="text" name="end_hour" size="5" value="<?php echo date("H", $nEndEpockSec);?>" />時<input type="text" name="end_min" size="5" value="<?php echo date("i", $nEndEpockSec);?>" />分<br />
	IPアドレス ： <input type="text" name="ip" size="40" />（<span style="color:red;">完全一致</span>。%は0文字以上の任意の文字、_は任意の1文字に一致。例：192.168.%）<br />
	ホスト名 ： <input type="text" name="hostname" size="60" />（<span style="color:red;">完全一致</span>。%は0文字以上の任意の文字、_は任意の1文字に一致。例：%ocn.ne.jp）<br />
	ブラウザ文字列 ： <input type="text" name="useragent" size="60" />（一部に一致。例：MSIE 8.0）<br />
	閲覧ページ ： <input type="text" name="referer" size="60" />（一部に一致）<br />
	言語 ： <input type="text" name="language" size="60" />（一部に一致。例：ja, en など）<br />
	ソート順 ： <input type="radio" name="sort" value="day_asc" />日時 降順<input type="radio" name="sort" value="day_desc" checked="checked"  /><span style="color:red;">日時 昇順</span><input type="radio" name="sort" value="ip" />IPアドレス</p>
	
	<p><input type="submit" value="クエリ開始" name="B1" /><input type="reset" value="表示項目のリセット" name="B2" /><input type="hidden" name="query_command" value="start" /></p>
</form>

<?php
}

$nSqlProcessTime = $flEndTime - $flStartTime;

print("<p>件数：".$nQuery."（データベース内データ数：".$nTotalData."）,処理時間：".$nSqlProcessTime." 秒</p>\n");

print("<p><a href=\"$strThisScriptName\">カレントDBクエリ入力画面を再表示する</a> | <a href=\"$strThisScriptName?db=backup\">バックアップDBクエリ入力画面を再表示する</a> | <a href=\"index.php\">メニューに戻る</a></p>\n");
?>
</body>

</html>

<?php
function ErrorDispAndExit($strMsg)
{
	print("<p>エラーが発生しました：$strMsg</p>\n<p><a href=\"$strThisScriptName\">入力画面を再表示する</a> | <a href=\"index.php\">メニューに戻る</a></p>\n</body>\n</html>\n");
	exit();
}

// SQLインジェクションを防ぐためのエスケープ処理

function EscapeSqlString($strSql)
{
	// （’や”を）エスケープする。addslashesではダメらしい
	$strSql = sqlite_escape_string($strSql);
	
	// セミコロンはSQL文末の記号のため削除する
	$strSql = str_replace(";", "", $strSql);
	
	return($strSql);
}

?>
