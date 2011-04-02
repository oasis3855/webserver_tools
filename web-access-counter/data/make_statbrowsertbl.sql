/* make statbrowsertbl table OR refresh initial data */
/* usage : sqlite3 accdb.sqlite < make_statbrowsertbl.sql */
create table if not exists statbrowsertbl (
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
delete from statbrowsertbl;

/* initial data */
insert into statbrowsertbl (pattern, real_name) values ('MSIE 4.', 'MSIE 4');
insert into statbrowsertbl (pattern, real_name) values ('MSIE 5.', 'MSIE 5.x');
insert into statbrowsertbl (pattern, real_name) values ('MSIE 6.', 'MSIE 6');
insert into statbrowsertbl (pattern, real_name) values ('MSIE 7.', 'MSIE 7');
insert into statbrowsertbl (pattern, real_name) values ('MSIE 8.', 'MSIE 8');
insert into statbrowsertbl (pattern, real_name) values ('MSIE 9.', 'MSIE 9');
insert into statbrowsertbl (pattern, real_name) values ('Firefox/2.', 'Firefox 2');
insert into statbrowsertbl (pattern, real_name) values ('Firefox/3.', 'Firefox 3');
insert into statbrowsertbl (pattern, real_name) values ('Firefox/4.', 'Firefox 4');
insert into statbrowsertbl (pattern, real_name) values ('Opera', 'Opera');
insert into statbrowsertbl (pattern, real_name) values ('Chrome/', 'Chrome');

