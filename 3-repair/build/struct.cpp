 #include <hls_stream.h>
struct If2 {
  hls::stream<int> &in;
  hls::stream<int> &out;
  If2(hls::stream<int> &i, hls::stream<int> &o) : in(i), out(o) {}

  int doRead() const {
   #pragma HLS INLINE off
    return in.read();
  }
  void doWrite(int v) const {
   #pragma HLS INLINE off
    out.write(v);
  }
  void do1() const {
    // do something with in and out
    doWrite(doRead() + 1);
  }
};
void top(hls::stream<int> &in, hls::stream<int> &out) {
	#pragma HLS DATAFLOW

	If2{ in, out}.do1();
}
