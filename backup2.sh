#!/bin/bash
src=$1
trgt=$2
k=$3
echo $src $trgt $k
d=`date`
date_no_space=`echo -e $d | sed -e 's/ /_/g'`
date_no_colons=`echo -e $date_no_space | sed -e 's/:/_/g'`
echo "The Date of the backup is: [ $d ]"
echo -e "The modified date is [ $date_no_colons ]\n"
if [[ $# -ne 3 ]]; then
	echo "Usage: ./backup2.sh source_directory destination_directory encryption_key"
else
	mkdir $trgt/$date_no_colons
	echo "[+] Starting the script now: "
	for d in $src/*/; do
		if [ -d $d ]; then
			dirname=`basename "${d}"`
			echo "dirname extracted: " $dirname
			new_f=$dirname"_"$date_no_colons
			tar_name=$new_f".tar.gz"
			tar -czf "$tar_name" -P $d
			gpg --pinentry-mode loopback --passphrase $k --symmetric $tar_name
			mv ./$tar_name.gpg $trgt/$date_no_colons
			shred -u $tar_name
		fi
	done
	echo -e "[+] Done encrypting and compressing: \n"

	tar -cf $date_no_colons"_FULL.tar" -C $trgt/$date_no_colons -T /dev/null
fi
