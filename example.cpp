#include "SQLite3Helper/sqlite3_helper.h"

#include <iostream>


using namespace SQLite3Helper;
using namespace std;


Query create_EXAMPLE = "create table if not exists EXAMPLE (id INTEGER primary key, data TEXT)";  // void -> void

Query insert_EXAMPLE_data = "insert into EXAMPLE (data) values (?)";  // string -> void
Query insert_id_EXAMPLE_data = "insert into EXAMPLE (data) values (?) returning id";  // string -> uint64

Query select_count_EXAMPLE = "select count(*) from EXAMPLE";  // void -> uint64
Query select_id_data_EXAMPLE = "select id, data from EXAMPLE";  // void -> vector<tuple<uint64, string>>
Query select_data_EXAMPLE_id = "select data from EXAMPLE where id = ?";  // uint64 -> string

Query update_EXAMPLE_data_id = "update EXAMPLE set data = ? where id = ?";  // string, uint64 -> void

Query delete_EXAMPLE_id = "delete from EXAMPLE where id = ?";  // uint64 -> void


int main() {
	Database db("example.db");
	db.Execute(create_EXAMPLE);

	db.Execute(insert_EXAMPLE_data, string("Hello"));

	uint64 id = db.ExecuteForOne<uint64>(insert_id_EXAMPLE_data, string("World"));
	string data = db.ExecuteForOne<string>(select_data_EXAMPLE_id, id);
	cout << id << " " << data << endl;

	db.Execute(update_EXAMPLE_data_id, string("World !"), id);

	for (auto [id, data] : db.ExecuteForMultiple<tuple<uint64, string>>(select_id_data_EXAMPLE)) {
		cout << id << " " << data << endl;
	}

	db.Execute(delete_EXAMPLE_id, id);

	cout << db.ExecuteForOne<uint64>(select_count_EXAMPLE) << endl;
}