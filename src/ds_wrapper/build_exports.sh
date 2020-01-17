#!/bin/bash

if [ "$#" -lt 3 ]; then
  echo "usage: build_exports.sh input output module1 module2 ..."
  echo "example: ./build_exports.sh ../ds/exports.txt exports.txt ../ds/modules/isoldr/ ../ds/modules/isofs/ ../ds/modules/minilzo/"
  exit 1
fi

# set variables
exp_input=$1
exp_output=$2
modules=()

# cleanup exports
rm -f "$2"

# skip first two args
shift
shift

# build modules list
while test ${#} -gt 0; do
  modules+=("$1")
  shift
done

# let's go
while IFS= read -r line; do

  # skip lines starting with #
  if [[ "$line" =~ ^#.* ]] || [[ "$line" == "rename" ]] ; then
    continue
  fi

  for module in "${modules[@]}"; do
    # search for export in module
    if grep -qr --include="*.c" "$line" "$module"; then
      # only add export if it doesn't exist yet in output file

      if grep -q "$line" "$exp_output"; then
        echo "$line: aleady added to export file"
      else
        echo "$line" >>"$exp_output"
      fi
      shift
    fi
  done

done <"$exp_input"
