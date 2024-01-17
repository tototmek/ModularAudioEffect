#! /bin/bash
files=0
failed=0
RED='\033[1;31m'
GREEN='\033[0;32m'
NC='\033[0m'

echo "Files to check:"
# changed_files=`git --no-pager diff origin/master --stat --color=always | grep -v dependencies/`
changed_files=$(find . -type f -path "./src/*.cpp")
echo "$changed_files"
for file in $changed_files;
do
  if [ ! -f $file ]; then
    continue
  fi
  echo    ""
  echo -e "Checking file: ${GREEN}$file${NC}"
  output=`clang-tidy-12 $file --quiet -p ./build 2>/dev/null`
  status=$?
  echo -n "${output}"
  let "failed+=$status"
  let "files+=1"
  if [[ "$status" -eq 0 ]]; then
    echo -e " Status: ${GREEN}Passed${NC}"
  else
    echo -e " Status: ${RED}Failed${NC}"
  fi
done

let "passed = $files - $failed"
echo ""
echo "$passed/$files files passed the check."
echo "Exit status $failed"
exit $failed


    