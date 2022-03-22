find mk \( -type d -name .git -prune \) -o -type f -name "*mk" -print0 | xargs -0 sed -i 's/$1.def/cfg\/$1.def/'

