function backup(){
src=$1
trgt=$2
k=$3
echo $src $trgt $k
d=`date`
date_no_space=`echo -e $d | sed -e 's/ /_/g'`
date_no_colons=`echo -e $date_no_space | sed -e 's/:/_/g'`
echo "The Date of the backup is: [ $d ]"
echo -e "The modified date is [ $date_no_colons ]\n"
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


	full=$trgt/$date_no_colons/$date_no_colons"_FULL.tar"

	(cd -P -- "$trgt/$date_no_colons" && printf '%s\0' *.gpg | tar --null -T - -zcvf -) > $full.gz

	cd $trgt/$date_no_colons

	echo "[+] Encrypting the tar that contains everything"
	gpg --pinentry-mode loopback --passphrase $k --symmetric $full.gz

	rm -v !("$date_no_colons""_FULL.tar.gz.gpg"|"minakkk.tar")
	echo "[+] DONE!"
	echo "_____________________________________________________"
	echo
}

function validate_backup_params(){
	if [[ $# -ne 3 ]]; then
		echo "Usage: ./test_backup.sh source_directory destination_directory encryption_key"
	elif [[ ! -d $1 ]]; then
    		echo "$1 doesn't exist on your filesystem."
	elif [[ ! -d $2 ]];
	then
    		echo "$2 doesn't exist on your filesystem."
	else backup $1 $2 $3
	fi

}

function restore(){
	src=$1
	dest=$2
	k=$3
	mkdir $dest/temp
	for f in $src/*.gpg; do
	[ -f "$f" ] || break
		echo $f
	name=`basename $f .gpg`
	echo $name
	gpg --pinentry-mode loopback --passphrase $k --output $dest/temp/"$name" --decrypt $f
	echo $dest/temp/"$name"
	tar -xf $dest/temp/"$name" -C $dest/temp/
	done

	for f in $dest/temp/*.gpg; do
	[ -f "$f" ] || break
		echo $f
	nom=$(basename $f .gpg)
	gpg --pinentry-mode loopback --passphrase $k --output $dest/temp/"$nom" --decrypt $f
	echo "[+] Decrypting $nom"
	echo -e "Will now delete $f \n"
	shred -u $f
	done

	cd $dest/temp/
	rm -rf *.gpg
	echo "currently at: "
	pwd
	for f in $dest/temp/*.gz; do
		tar -xf $f -C $dest/temp
		echo "[-] Deleting $f"
		rm -rf $f
		rm -rf $f.gpg
	done
}

function validate_restore_params(){
	if [[ $# -ne 3 ]]; then
		echo "Usage: ./restore.sh backup_directory restore_directory restoration_key"
	elif [[ ! -d $1 ]]; then
    		echo "$1 doesn't exist on your filesystem."
	elif [[ ! -d $2 ]];
	then
    		echo "$2 doesn't exist on your filesystem."
	else restore $1 $2 $3
	fi
}



