#! /usr/bin/env sh

dir="$1"
file="`basename $2`"
name="`awk -F '/' '{print($(NF-1));}' <<< ${dir}`"
font='slant'

if [ ! "${dir}" -o ! "${file}" ]; then
  exit 1
fi

if [ -e "${dir}/LICENSE" ]; then
  license="`awk '{printf(\" * %s\n\", $0);}' ${dir}/LICENSE`"
elif [ -e "${dir}/../LICENSE" ]; then
  license="`awk '{printf(" * %s\n", $0);}' ${dir}/../LICENSE`"
fi

if [ -e ${dir}/figletfont ]; then
  font=`cat ${dir}/figletfont`
fi

if [ -e ${dir}/projectname ]; then
  name="`cat ${dir}/projectname`"
fi
if [ -n ${name} ]; then
  name="`figlet -f ${font} ${name} | awk '{printf(" * %s\n", $0)}'`"
fi

echo "/*"
echo " * ${file}"
if [ -n "${license}" ]; then
  echo " *"
  echo "${license}"
fi
if [ -n "${name}" ]; then
  echo " *"
  echo "${name}"
fi
echo " *"
echo " */"
