
C:\Jp2_pc\Source\Lib\EntityDBase\WorldDBase.cpp(257) : error C2247: 'pr3PresenceWritable' not accessible because 'CPartition' uses 'public' to inherit from 'CPArray<char>'
//
// Um...............WHAT!!!???
// This message makes no sense whatsoever... 
// The item is indeed protected; but CPartition does NOT inherit from CPArray<char>, or even use it at all.
//
