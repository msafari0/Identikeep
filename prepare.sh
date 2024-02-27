mkdir -p archives

wget -O rapidjson.zip https://github.com/Tencent/rapidjson/archive/master.zip
unzip rapidjson.zip rapidjson-master/include/*
mv rapidjson-master/include/rapidjson include/
rmdir rapidjson-master/include && rmdir rapidjson-master
mv rapidjson.zip archives/

wget -O argh.zip  https://github.com/adishavit/argh/archive/v1.2.0.zip
unzip argh.zip  argh-1.2.0/argh.h
mv argh-1.2.0/argh.h include/
rmdir argh-1.2.0
mv argh.zip archives/

wget -O plog.zip https://github.com/SergiusTheBest/plog/archive/refs/tags/1.1.4.zip
unzip plog.zip 
mv plog-1.1.4/include/plog include/
rm -r plog-1.1.4
mv plog.zip archives/



