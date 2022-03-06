## Webサーバ管理用ソフトウエア（Windows, Linux, BSD）<br />Web server side scripts<!-- omit in toc -->

[Home](https://oasis3855.github.io/webpage/) > [Software](https://oasis3855.github.io/webpage/software/index.html) > [Software Download](https://oasis3855.github.io/webpage/software/software-download.html) > ***webserver_tools*** (this page)

<br />
<br />

- [Apache Webサーバのログからエラー行を抽出してHTML化するPerlスクリプト (Linux, BSD)](#apache-webサーバのログからエラー行を抽出してhtml化するperlスクリプト-linux-bsd)
- [AutoLogParser（IIS/ApacheログをMicrosoft Log Parserで解析する自動化スクリプト生成ツール）(Windows)](#autologparseriisapacheログをmicrosoft-log-parserで解析する自動化スクリプト生成ツールwindows)
- [ExecAnalog（Analogに今月・先月のログ解析をさせるための自動化ツール）(Windows)](#execanaloganalogに今月先月のログ解析をさせるための自動化ツールwindows)
- [ReadNsLog（NetScreenのログからエラー行を抽出するツール）(Windows)](#readnslognetscreenのログからエラー行を抽出するツールwindows)
- [ReadEvent（Windowsイベント ログからエラー行を抽出しHTML化するツール）(Windows)](#readeventwindowsイベント-ログからエラー行を抽出しhtml化するツールwindows)
- [Webページ用アクセスログ記録とログ検索スクリプト (Linux, BSD)](#webページ用アクセスログ記録とログ検索スクリプト-linux-bsd)
- [Visual C++ .NET 2003 MFC 7.1 共有dll](#visual-c-net-2003-mfc-71-共有dll)


<br />
<br />

## Apache Webサーバのログからエラー行を抽出してHTML化するPerlスクリプト (Linux, BSD)

Apache Webサーバより出力されたsyslogから、HTTPリザルトコード300以上のもの及びエラーログを抽出し、見やすいHTML形式に成形するPerlスクリプト。cronで毎日のログローテーション後に実行する自動化ツールとして作成しています。

[配布ディレクトリ apache-log-html-converter](apache-log-html-converter/README.md) (2012/02/21)


<br />
<br />

## AutoLogParser（IIS/ApacheログをMicrosoft Log Parserで解析する自動化スクリプト生成ツール）(Windows)

IISログ（W3C拡張形式/IIS方式）またはApache Web Serverログ（NCSA形式）をMicrosoft Log Parserを用いて解析するための自動化補助プログラムです。

[配布ディレクトリ auto_logparser](auto_logparser/README.md) (2012/02/11)

<br />
<br />

## ExecAnalog（Analogに今月・先月のログ解析をさせるための自動化ツール）(Windows)

このプログラムは、ログ解析ツール[Analog](https://en.wikipedia.org/wiki/Analog_(program))に、今月および前月のログを解析させるための自動化ツールです。

[配布ディレクトリ exec_analog](exec_analog/README.md) (2007/07/25)

<br />
<br />

## ReadNsLog（NetScreenのログからエラー行を抽出するツール）(Windows)

Syslogに書き込まれたNetScreen（ファイアウォール機器）のログファイルから、エラーや警告などのログ行を抜き出してテキスト化するツールです。

[配布ディレクトリ read_netscreen_log](read_netscreen_log/README.md) (2007/07/25)

<br />
<br />

## ReadEvent（Windowsイベント ログからエラー行を抽出しHTML化するツール）(Windows)

このプログラムは、Windowsのイベントログからエラー等の行を抽出しHTML化するツールです。

また、Symantec Backup Exec の動作ログを抽出することもできます。

[配布ディレクトリ read_windows_event](read_windows_event/README.md) (2007/07/25)

<br />
<br />

## Webページ用アクセスログ記録とログ検索スクリプト (Linux, BSD)

webページを表示した端末のログをSQLite3（SQLデータベース）に記録し、またその記録を検索するためのスクリプト集。作者のWebページで10年以上、実際に利用中のスクリプト。

[配布ディレクトリ web-access-counter](web-access-counter/README.md) (2007/07/25)

<br />
<br />

---

<br />
<br />

## Visual C++ .NET 2003 MFC 7.1 共有dll

Visual C++ .NET 2003 の MFC 7.1 を用いて作成した Windows プログラムの実行に必要な共有dllファイルを再配布します。

[配布ディレクトリ mfc71_dll](mfc71_dll/README.md)
