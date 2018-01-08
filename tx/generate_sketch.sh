DIR=$(pwd)
for f in *
    do
        ln -s $DIR/${f} ~/sketchbook/transmitter/${f}
    done

