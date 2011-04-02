/* make statostbl table OR refresh initial data */
/* usage : sqlite3 accdb.sqlite < make_statostbl.sql */
create table if not exists statostbl (
	idx				integer primary key autoincrement,
	pattern			varchar(128) not null,
	real_name		varchar(128) not null,
	data_month_1	integer
	data_month_2	integer
	data_month_3	integer
	data_month_4	integer
	data_month_5	integer
	data_month_6	integer
	data_month_7	integer
	data_month_8	integer
	data_month_9	integer
	data_month_10	integer
	data_month_11	integer
	data_month_12	integer
);

/* delete all data */
delete from statostbl;

/* initial data */
insert into statostbl (pattern, real_name) values ('Windows 95', 'Windows 95');
insert into statostbl (pattern, real_name) values ('Windows 98', 'Windows 98');
insert into statostbl (pattern, real_name) values ('Windows NT 5.0', 'Windows 2000');
insert into statostbl (pattern, real_name) values ('Windows NT 5.1', 'Windows XP');
insert into statostbl (pattern, real_name) values ('Windows NT 5.2', 'Windows Server 2003');
insert into statostbl (pattern, real_name) values ('Windows NT 6.0', 'Windows Vista/ Server 2008');
insert into statostbl (pattern, real_name) values ('Windows NT 6.1', 'Windows 7/ Server 2008R1');
insert into statostbl (pattern, real_name) values ('Macintosh U', 'Mac OS X');
insert into statostbl (pattern, real_name) values ('iPhone OS', 'iPhone OS');
insert into statostbl (pattern, real_name) values ('Linux', 'Linux');

