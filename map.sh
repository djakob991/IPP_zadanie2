#!/bin/bash

re='^[0-9]+$'
count=0

for item in "$@"
do
  ((count++))
  if (( $count == 1))
  then
    if ! test -f $item; then exit 1; fi
  else
    if ! [[ $item =~ $re ]]; then exit 1; fi
    if (( ${#item} > 3 || ${item:0:1} == 0 )); then exit 1; fi
  fi
done
if (( $count < 2 )); then exit 1; fi

count=0
for item in "$@"
do
  ((count++))
  if (( count != 1 ))
  then
    for line in $(cat $1)
    do
      routeId=${line%%;*}
      if (($routeId == $item))
      then
        length=0
        rest=${line#*;}
        rest=${rest#*;}
        semi="${rest//[^;]}"
        semi_count=${#semi}

        while (( $semi_count > 0 ))
        do
          number=${rest%%;*}
          (( length = length + $number ))
          (( semi_count = semi_count - 3))
          for i in {1..3}; do rest=${rest#*;}; done
        done
        echo "$routeId;$length"
      fi
    done
  fi
done
exit 0
