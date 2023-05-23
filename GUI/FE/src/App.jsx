import { Box, Button, Stack } from '@mui/material';
import Tab from '@mui/material/Tab';
import TabContext from '@mui/lab/TabContext';
import TabList from '@mui/lab/TabList';
import TabPanel from '@mui/lab/TabPanel';
import { useState } from 'react';
import ComputerIcon from '@mui/icons-material/Computer';
import UploadFileIcon from '@mui/icons-material/Upload';
import LoadingButton from '@mui/lab/LoadingButton';

import axios from 'axios';

function LabTabs({ quadruples, symbolTable }) {
	const [value, setValue] = useState('1');

	const handleChange = (event, newValue) => {
		setValue(newValue);
	};

	return (
		<Box
			sx={{
				typography: 'body1',
				flex: 1,
				maxHeight: '100%',
				maxWidth: '50%',
			}}
		>
			<TabContext value={value}>
				<Box sx={{ borderBottom: 1, borderColor: 'divider' }}>
					<TabList onChange={handleChange}>
						<Tab label="Quadrables" value="1" />
						<Tab label="Symbol Table" value="2" />
					</TabList>
				</Box>
				<TabPanel
					value="1"
					sx={{
						height: '100%',
						maxHeight: '60vh',
					}}
				>
					<Box
						sx={{
							flex: 1,
							border: '1px solid black',
							p: 1,
							px: 2,
							height: '94%',
							maxHeight: '94%',
							overflow: 'auto',
						}}
					>
						<h2>Quadrables</h2>
						{/* {quadruples && <pre>{quadruples}</pre>} */}
						{quadruples.split('\n').map((line, index) => (
							<Box key={index} sx={{}}>
								<Box component={'pre'} display="flex" gap={2} py={0} my={0}>
									<Box bgcolor={'grey.300'}>
										{(index + 1).toString().padStart(2, '0')}
									</Box>
									<span>{line}</span>
								</Box>
							</Box>
						))}
					</Box>
				</TabPanel>
				<TabPanel
					value="2"
					sx={{
						height: '100%',
						maxHeight: '60vh',
					}}
				>
					<Box
						sx={{
							flex: 1,
							border: '1px solid black',
							p: 1,
							px: 2,
							height: '94%',
							maxHeight: '94%',
							overflow: 'auto',
						}}
					>
						<h2>Symbol Table</h2>
						{symbolTable.split('\n').map((line, index) => (
							<Box key={index} sx={{}}>
								<Box component={'pre'} display="flex" gap={2} py={0} my={0}>
									<Box bgcolor={'grey.300'}>
										{(index + 1).toString().padStart(2, '0')}
									</Box>
									<span>{line}</span>
								</Box>
							</Box>
						))}
					</Box>
				</TabPanel>
			</TabContext>
		</Box>
	);
}

function App() {
	const [file, setFile] = useState(null);
	const [fileContents, setFileContents] = useState('');
	const [errors, setErrors] = useState([]);
	const [quadruples, setQuadruples] = useState('');
	const [symbolTable, setSymbolTable] = useState('');
	const [linesTohighlight, setLinesToHighlight] = useState([]);
	const [loading, setLoading] = useState(false);

	const handleFileChange = (event) => {
		const selectedFile = event.target.files[0];
		setFile(selectedFile);
		const reader = new FileReader();
		reader.readAsText(selectedFile);
		reader.onload = () => {
			setFileContents(reader.result);
		};

		console.log('file name', selectedFile.name);

		// clear all
		setErrors([]);
		setQuadruples('');
		setSymbolTable('');
		setLinesToHighlight([]);
	};

	const handleCompile = async () => {
		if (!file?.name) {
			alert('Please select a file');
			return;
		}

		setLoading(true);

		try {
			const { data } = await axios.post('http://localhost:3000/compile', {
				inputFile: file.name,
			});

			setErrors(data?.errors?.split('\n').filter((e) => e));
			setQuadruples(data?.quadruples);
			setSymbolTable(data?.symbolTable);

			const lines = [];
			const regex = /\[(\d+)\]/g;
			let match;
			while ((match = regex.exec(data.errors)) !== null) {
				const lineNumber = match[1];
				lines.push(parseInt(lineNumber));
			}

			setLinesToHighlight(lines);
			console.log(lines);

			console.log(data);
		} catch (error) {
			console.log(error);
			alert('Something went wrong');
		}

		setLoading(false);
	};

	return (
		<Box mb={8}>
			{/* Header */}
			<Box
				sx={{
					width: '100%',
					bgcolor: 'primary.main',
					color: 'primary.contrastText',
					p: 2,
				}}
			>
				<Box
					sx={{
						display: 'flex',
						justifyContent: 'space-between',
						alignItems: 'center',
						width: '80%',
						margin: 'auto',
					}}
				>
					<h1>Compiler </h1>
					<ComputerIcon
						sx={{
							fontSize: '4rem',
						}}
					/>
				</Box>
			</Box>

			{/* Body */}
			<Box
				sx={{
					height: '100%',
					width: '80%',
					margin: 'auto',
					mt: 2,
				}}
			>
				<Box
					sx={{
						display: 'flex',
						justifyContent: 'space-between',
						minHeight: '50vh',
						gap: 3,
					}}
				>
					<Box
						sx={{
							flex: 1,
							border: '1px solid black',
							p: 1,
							px: 2,
							height: '60vh',
							maxHeight: '60vh',
							overflow: 'auto',
						}}
					>
						<h2>code</h2>
						{fileContents &&
							// <pre>{fileContents}</pre>
							fileContents.split('\n').map((line, index) => {
								const lineNumber = index + 1;
								console.log(linesTohighlight.includes(lineNumber));

								return (
									<Box
										key={index}
										// component={"pre"}
										sx={{
											py: 0,
											my: 0,
											display: 'flex',
											// justifyContent: 'space-between',
											gap: 2,
											alignItems: 'center',
											bgcolor: linesTohighlight.includes(index + 1)
												? 'error.main'
												: 'transparent',
										}}
									>
										<Box component={'pre'} display="flex" gap={1} py={0} my={0}>
											<Box bgcolor={'grey.300'}>
												{(index + 1).toString().padStart(2, '0')}
											</Box>
											<span>{line}</span>
										</Box>
									</Box>
								);
							})}
					</Box>
					<LabTabs quadruples={quadruples} symbolTable={symbolTable} />
				</Box>

				{/* Erros & submit */}
				<Box
					sx={{
						display: 'flex',
						justifyContent: 'space-between',
						gap: 3,
						mt: 3,
					}}
				>
					<Box sx={{ flex: 6, border: '1px solid black', p: 1, px: 2 }}>
						<h2>Errors</h2>
						{errors?.length > 0 ? (
							<ul>
								{errors.map((error, index) => (
									<li key={index}>{error}</li>
								))}
							</ul>
						) : (
							<Box color="success.main">
								{quadruples.length > 0 || symbolTable.length > 0
									? 'No errors found'
									: ''}
							</Box>
						)}
					</Box>
					<Stack spacing={2} flex={1} justifyContent={'center'}>
						<Button
							variant="contained"
							component="label"
							startIcon={<UploadFileIcon />}
						>
							Upload File
							<input type="file" hidden onChange={handleFileChange} />
						</Button>
						<LoadingButton
							variant="contained"
							color="success"
							startIcon={<ComputerIcon />}
							onClick={handleCompile}
							loading={loading}
						>
							Compile
						</LoadingButton>
					</Stack>
				</Box>
			</Box>
		</Box>
	);
}

export default App;
