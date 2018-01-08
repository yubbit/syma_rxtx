DIR=$(pwd)
for f in *
    do
        ln -s $DIR/${f} ~/sketchbook/listener/${f}
    done

