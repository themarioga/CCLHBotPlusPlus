cd CCLHBotCommons
make clean && make -j4
sudo make install
cd ..

cd CCLHBotDictionaries
make clean && make -j4
cd ..

cd CCLHBotPlusPlus
make clean && make -j4
cd ..