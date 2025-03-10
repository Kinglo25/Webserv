document.addEventListener('DOMContentLoaded', () => {
	let data = [];
	let loaded = false;
	let errorMsg = null;

	const fileInput = document.querySelector('#fileInput');

	const sendFile = async () => {
		const file = fileInput.files[0];

		if (!file) {
			fileInput.value = null;
			return;
		}

		const formData = new FormData();
		formData.append('image', file);

		try {
			const response = await axios.post(`${location.origin}/api`,
				formData,
				{
					headers: {
						'Content-Type': 'multipart/form-data'
					}
				}
			);

			data = [...response.data];
			render();
		} catch (err) {
			console.error(err);
			errorMsg = "couldn't send the file";
		}
		fileInput.value = null;
	}

	const deleteFile = async (file) => {
		try {
			const response = await axios.delete(`${location.origin}/api`, {
				data: { file: file }
			});

			data = [...response.data];
			render();
		}
		catch (err) {
			console.error(err);
			errorMsg = "couldn't remove " + file;
		}
	}

	const loadData = async () => {
		loaded = true;
		try {
			const response = await axios.get(`${location.origin}/api`);

			data.push(...response.data);
			console.log(response.data);
			render();
		}
		catch (err) {
			console.error(err);
			errorMsg = "couldn't download images";
			data.length = 0;
		}
	}

	const render = () => {
		const errCont = document.querySelector('#error');
		errCont.innerHTML = '';
		
		if (errorMsg) {
			errCont.innerHTML = `${errorMsg}`;
			errorMsg = null;
		}
		
		const container = document.querySelector('#images');
		container.innerHTML = '';
		
		data.forEach((file) => {
			const div = document.createElement('div');
			div.classList.add('image');
			
			const img = document.createElement('img');
			img.src = `${file}?t=${Date.now()}`;
			div.appendChild(img);
			console.log("img", img);
			
			const inp = document.createElement('input');
			inp.type = 'button';
			inp.value = 'Delete';
			inp.addEventListener('click', () => {
				deleteFile(file);
			});
			inp.classList.add('deleteFileButton');

			div.appendChild(inp);
			container.appendChild(div);
		});
	}

	loadData();

	document.querySelector('#sendFileButton').addEventListener('click', sendFile);
});
