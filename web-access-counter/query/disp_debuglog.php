<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="ja" lang="ja">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=shift_jis" />
	<meta http-equiv="Content-Language" content="ja" />
	<title>web-counter debug log (data/debuglog.csv)</title>
</head>
<body>

<?php

print("<p>�f�o�b�O���O�itail ../data/debuglog.csv �R�}���h�j</p>\n");
print("<pre>\n");

$strResult = system('tail -n 40 ../data/debuglog.csv');
print($strResult);
print("</pre>\n");

print("\n\n�\������\n");

?>
</pre>

<p>&nbsp;</p>
<p><a href="index.php">���j���[�\���ɖ߂�</a></p>

</body>
</html>
