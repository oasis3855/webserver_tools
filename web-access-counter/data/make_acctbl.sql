/* make acctbl table */
/* usage : sqlite3 accdb.sqlite < make_acctbl.sql */
create table if not exists acctbl (
	idx			integer primary key autoincrement,
	tm_epock	integer not null,
	weekday		integer not null,
	tm_day_sec	integer not null,
	ip			varchar(16) not null,
	hostname	varchar(256),
	browser		varchar(256),
	referer		varchar(256),
	lang		varchar(64)
);

