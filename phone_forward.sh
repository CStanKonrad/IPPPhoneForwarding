 #/bin/bash

#Wypisuje wiadomość zawartą pod $1.
#Usuwa pliki których nazwy znajdują się pod argumentami $i gdzie i >= 2.
#Kończy skrypt kodem 1.
function errorFinishScript {
	for i in ${@:2}
	do
		rm "$i"
	done
	echo -e "$1"
	exit 1
}


#Usuwa pliki których nazwy znajdują się pod argumentami $i gdzie i >= 1.
#Kończy skrypt kodem 0.
function finishScript {
	for i in ${@}
	do
		rm "$i"
	done
	exit 0
}

function isNumber {
	
	if [ "$#" != '1' ]
	then
		echo 'false'
	else
		REGEX='^[0-9]+$'
		
		if [[ $1 =~ $REGEX ]]
		then
			echo 'true'
		else
			echo 'false'
		fi
	fi
}

if [ "$#" != "3" ]
then
	errorFinishScript 'Zła liczba argumentów oczekiwano <prog> <plik> <numer>'
fi
 
PROGRAM_PATH=$1
FILE=$2
NUMBER=$3


numberTest=$(isNumber $NUMBER)
if [ $(isNumber $NUMBER) != 'true' ]
then
	errorFinishScript "$NUMBER nie reprezentuje numeru"
fi

TMP_ERROR_MESSAGE="Nie udało się stworzyć pliku tymczasowego."

TMP_INPUT=$(mktemp XXXXXXXXXXX.tmp.in) \
|| errorFinishScript "$TMP_ERROR_MESSAGE"
tmpFiles=( $TMP_INPUT )

TMP_RAW_OUTPUT=$(mktemp XXXXXXXXXXX.tmp.proc) \
|| errorFinishScript "$TMP_ERROR_MESSAGE" ${tmpFiles[@]}
tmpFiles+=( $TMP_RAW_OUTPUT )

TMP_NEW_INPUT=$(mktemp XXXXXXXXXXX.tmp.in) \
|| errorFinishScript "$TMP_ERROR_MESSAGE" ${tmpFiles[@]}
tmpFiles+=( $TMP_NEW_INPUT )

TMP_NEW_OUTPUT=$(mktemp XXXXXXXXXXX.tmp.out) \
|| errorFinishScript "$TMP_ERROR_MESSAGE" ${tmpFiles[@]}
tmpFiles+=( $TMP_NEW_OUTPUT )

TMP_CONCAT=$(mktemp XXXXXXXXXXX.tmp.out) \
|| errorFinishScript "$TMP_ERROR_MESSAGE" ${tmpFiles[@]}
tmpFiles+=( $TMP_CONCAT )


function clean {
	finishScript ${tmpFiles[@]}
}

trap clean EXIT

echo "NEW BASE " > $TMP_INPUT
cat "$FILE" >> $TMP_INPUT
echo "? $NUMBER" >> $TMP_INPUT



cmd="\"$PROGRAM_PATH\" < $TMP_INPUT > $TMP_RAW_OUTPUT"

eval "$cmd"

echo "NEW BASE " > $TMP_NEW_INPUT
cat "$FILE" >> $TMP_NEW_INPUT

tr '\n' '?' < $TMP_RAW_OUTPUT >> $TMP_NEW_INPUT

sed -i -e 's/?/ ?\n/g' $TMP_NEW_INPUT

cmd="\"$PROGRAM_PATH\" < $TMP_NEW_INPUT > $TMP_NEW_OUTPUT"
eval "$cmd"

paste $TMP_NEW_OUTPUT $TMP_RAW_OUTPUT > $TMP_CONCAT
sed -i -e "s/	/.	/g" $TMP_CONCAT


grep -w -e "$NUMBER." $TMP_CONCAT > $TMP_NEW_OUTPUT
sed -i -e "s/$NUMBER.	//g" $TMP_NEW_OUTPUT
cat $TMP_NEW_OUTPUT
