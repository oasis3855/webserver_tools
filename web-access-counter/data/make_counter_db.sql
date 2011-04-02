/* make counter DB */
/* usage : sqlite3 counter.sqlite < make_counter_db.sql */

drop table if exists countertbl;

create table countertbl (
	idx				integer primary key,
	counter			integer not null
);

insert into countertbl (idx, counter) values ('0', '0');


drop table if exists locktbl;

create table locktbl (
	idx				integer primary key,
	tm_epock		integer not null
);

insert into locktbl (idx, tm_epock) values ('0', '0');


drop table if exists iptbl;

create table iptbl (
	idx				integer primary key autoincrement,
	ip			varchar(16) not null,
	tm_epock	integer not null
);

insert into iptbl (ip, tm_epock) values ('0.0.0.0', '0');
insert into iptbl (ip, tm_epock) values ('0.0.0.0', '0');
insert into iptbl (ip, tm_epock) values ('0.0.0.0', '0');
insert into iptbl (ip, tm_epock) values ('0.0.0.0', '0');
insert into iptbl (ip, tm_epock) values ('0.0.0.0', '0');
insert into iptbl (ip, tm_epock) values ('0.0.0.0', '0');
insert into iptbl (ip, tm_epock) values ('0.0.0.0', '0');
insert into iptbl (ip, tm_epock) values ('0.0.0.0', '0');
insert into iptbl (ip, tm_epock) values ('0.0.0.0', '0');
insert into iptbl (ip, tm_epock) values ('0.0.0.0', '0');



