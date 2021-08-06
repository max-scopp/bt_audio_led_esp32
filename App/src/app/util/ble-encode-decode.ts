const encoder = new TextEncoder();
const decoder = new TextDecoder();

export const encode = (v: any) => encoder.encode(JSON.stringify(v));

export const decode = (buff: BufferSource) => {
  const strDecoded = decoder.decode(buff);
  try {
    return JSON.parse(strDecoded);
  } catch (e) {
    console.warn('decode guessing json failed: ', e);
    return strDecoded;
  }
};
