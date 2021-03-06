# Image Steganography

Steganography is a method of hiding secret data by embedding it into an audio, video, image. In this project BMP image is used to encode secret data in the the text file. 

## Orginal image used for encoding secret data 
![alt text](https://raw.githubusercontent.com/srsaidas/image-steganography/master/beautiful.bmp)

## View Secret Data 
```
cat secret.txt
```
![alt text](https://raw.githubusercontent.com/srsaidas/image-steganography/master/image/image_1.png)

# Encoding
```
./a.out -e <image_file> <secret_text_file> <desination_image_name>
```
-e is used for encoding

-d is used for decoding 

for encoding we need to pass BMP image and secret text file and destination image file which is optional 
```
./a.out -e beautiful.bmp secret.txt new.bmp
```

or 
```
./a.out -e beautiful.bmp secret.txt 
```
if destination image name is not specified stego_image.bmp is created 

![alt text](https://raw.githubusercontent.com/srsaidas/image-steganography/master/image/imag_2.png)

# Decoding 

![alt text](https://raw.githubusercontent.com/srsaidas/image-steganography/master/stego_image.bmp)
*Encoded Image*, our eye cannot see the difference
For decoding 
```
./a.out -d <encoded_image> <decoded_text>
```
decoded text is optional if not specified decoded.txt will created
```
./a.out -d stego_image.bmp decoded_text.txt
```

or 

```
./a.out -d stego_image.bmp 

```

![alt text](https://raw.githubusercontent.com/srsaidas/image-steganography/master/image/image_3.png)

# View Extracted secret data
 
```
cat decoded_text.txt
```
![alt text](https://raw.githubusercontent.com/srsaidas/image-steganography/master/image/imag4.png)



~Thanks

[Saidas S R!](https://www.linkedin.com/in/saidassr/)
